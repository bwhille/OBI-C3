#include <Arduino.h>
#include <Update.h>
#include <WebServer.h>

#undef setup
#undef loop

extern WebServer server;
extern void obi_base_setup();
extern void obi_base_loop();

#ifndef ESP_EN_PIN
#define ESP_EN_PIN 0
#endif

namespace {

bool ota_upload_started = false;
bool ota_upload_failed = false;
String ota_error;

const char OTA_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>OBI-C3 Firmware-Update</title>
  <style>
    :root{color-scheme:dark;--bg:#0b0e11;--panel:#171c22;--line:#2a333d;
      --text:#f4f7f9;--muted:#9ba8b4;--yellow:#ffb000;--green:#42c983;
      --red:#ff5b5b;font-family:system-ui,-apple-system,"Segoe UI",sans-serif}
    *{box-sizing:border-box}body{margin:0;background:radial-gradient(circle at 10% 0%,#ffb00015,transparent 28rem),var(--bg);color:var(--text)}
    main{width:min(720px,calc(100% - 28px));margin:40px auto}.panel{background:linear-gradient(145deg,#1c232b,var(--panel));border:1px solid var(--line);border-radius:15px;padding:24px;box-shadow:0 14px 35px #0003}
    .eyebrow{color:var(--yellow);font-size:.75rem;font-weight:800;letter-spacing:.13em;text-transform:uppercase;margin:0 0 4px}h1{margin:0 0 8px;font-size:clamp(1.6rem,5vw,2.2rem)}p{color:var(--muted);line-height:1.55}.warning{border:1px solid #ffb00077;background:#ffb0000d;border-radius:10px;padding:14px;margin:18px 0}.warning strong{color:#ffd36a}
    label{display:block;color:var(--muted);font-size:.86rem;margin:14px 0 6px}input,button{width:100%;min-height:46px;border-radius:9px;border:1px solid var(--line);padding:10px 13px;font:inherit}input{background:#0d1116;color:var(--text)}button{margin-top:16px;background:var(--yellow);color:#17120a;border:0;font-weight:800;cursor:pointer}button:disabled{opacity:.45;cursor:not-allowed}
    progress{width:100%;height:16px;margin-top:18px}.status{margin-top:14px;padding:12px;border-radius:9px;background:#0d1116;border:1px solid var(--line);min-height:46px}.ok{border-color:#42c98388;color:#9ff0c7}.error{border-color:#ff5b5baa;color:#ffb2b2}.links{display:flex;gap:10px;margin-top:18px}.links a{color:#ffca4b;text-decoration:none}
    code{background:#0d1116;border:1px solid var(--line);padding:2px 5px;border-radius:5px}
  </style>
</head>
<body>
<main>
  <section class="panel">
    <p class="eyebrow">OBI-C3 Wartung</p>
    <h1>Firmware-Update</h1>
    <p>Hier kannst du eine von PlatformIO erzeugte <code>firmware.bin</code> direkt über das Netzwerk installieren.</p>
    <div class="warning"><strong>Vor dem Update:</strong> Akku aus der Aufnahme entfernen, ESP32 stabil über USB-C versorgen und die Stromversorgung bis zum automatischen Neustart nicht unterbrechen.</div>
    <form id="form">
      <label for="firmware">Firmware-Datei</label>
      <input id="firmware" name="firmware" type="file" accept=".bin,application/octet-stream" required>
      <label for="confirmation">Zur Bestätigung UPDATE eingeben</label>
      <input id="confirmation" autocomplete="off" placeholder="UPDATE" required>
      <button id="submit" type="submit">Firmware installieren</button>
    </form>
    <progress id="progress" value="0" max="100" hidden></progress>
    <div id="status" class="status">Bereit.</div>
    <div class="links"><a href="/">← Zurück zur Diagnose</a></div>
  </section>
</main>
<script>
const form=document.querySelector('#form'),file=document.querySelector('#firmware'),confirmation=document.querySelector('#confirmation'),button=document.querySelector('#submit'),progress=document.querySelector('#progress'),statusBox=document.querySelector('#status');
function status(message,type=''){statusBox.textContent=message;statusBox.className='status '+type}
form.addEventListener('submit',event=>{
  event.preventDefault();
  if(confirmation.value!=='UPDATE'){status('Bitte UPDATE exakt eingeben.','error');return}
  if(!file.files.length||!file.files[0].name.toLowerCase().endsWith('.bin')){status('Bitte eine gültige firmware.bin auswählen.','error');return}
  if(!confirm('Firmware jetzt installieren? Die Verbindung wird beim Neustart kurz getrennt.'))return;
  const data=new FormData();data.append('firmware',file.files[0]);
  const xhr=new XMLHttpRequest();
  xhr.open('POST','/api/firmware?confirmation=UPDATE');
  xhr.upload.onprogress=e=>{if(!e.lengthComputable)return;progress.hidden=false;progress.value=Math.round(e.loaded/e.total*100);status('Upload läuft: '+progress.value+' %')};
  xhr.onload=()=>{let body={};try{body=JSON.parse(xhr.responseText)}catch{}if(xhr.status>=200&&xhr.status<300){progress.value=100;status(body.message||'Update erfolgreich. Gerät startet neu.','ok');button.disabled=true;setTimeout(()=>location.href='/',8000)}else{status(body.error||('Update fehlgeschlagen (HTTP '+xhr.status+').'),'error');button.disabled=false}};
  xhr.onerror=()=>{status('Netzwerkfehler während des Updates. Prüfe nach einigen Sekunden, ob das Gerät neu gestartet ist.','error');button.disabled=false};
  button.disabled=true;status('Firmware wird übertragen …');xhr.send(data);
});
</script>
</body>
</html>
)HTML";

void reset_ota_state() {
    ota_upload_started = false;
    ota_upload_failed = false;
    ota_error = "";
}

void fail_ota(const String &message) {
    ota_upload_failed = true;
    ota_error = message;
    Serial.println("OTA-Fehler: " + message);
}

void handle_ota_upload() {
    HTTPUpload &upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        reset_ota_state();
        digitalWrite(ESP_EN_PIN, LOW);

        if (!server.hasArg("confirmation") || server.arg("confirmation") != "UPDATE") {
            fail_ota("Bestaetigung fehlt. UPDATE muss eingegeben werden.");
            return;
        }

        if (!upload.filename.endsWith(".bin")) {
            fail_ota("Nur .bin-Firmwaredateien werden akzeptiert.");
            return;
        }

        Serial.println("OTA-Webupdate startet: " + upload.filename);
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
            fail_ota("Update konnte nicht gestartet werden: " + String(Update.errorString()));
            return;
        }
        ota_upload_started = true;
        return;
    }

    if (ota_upload_failed || !ota_upload_started) {
        return;
    }

    if (upload.status == UPLOAD_FILE_WRITE) {
        const size_t written = Update.write(upload.buf, upload.currentSize);
        if (written != upload.currentSize) {
            fail_ota("Firmware konnte nicht vollstaendig geschrieben werden: " + String(Update.errorString()));
            Update.abort();
        }
        return;
    }

    if (upload.status == UPLOAD_FILE_END) {
        if (!Update.end(true)) {
            fail_ota("Firmwareabschluss fehlgeschlagen: " + String(Update.errorString()));
            return;
        }
        Serial.printf("OTA-Webupdate abgeschlossen: %u Byte\n", upload.totalSize);
        return;
    }

    if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.abort();
        fail_ota("Upload wurde abgebrochen.");
    }
}

void handle_ota_result() {
    server.sendHeader("Cache-Control", "no-store");
    server.sendHeader("Connection", "close");

    if (ota_upload_failed || !ota_upload_started || Update.hasError()) {
        const String message = ota_error.length() > 0
            ? ota_error
            : "Firmwareupdate ist fehlgeschlagen.";
        server.send(500, "application/json; charset=utf-8", "{\"error\":\"" + message + "\"}");
        reset_ota_state();
        return;
    }

    server.send(
        200,
        "application/json; charset=utf-8",
        "{\"message\":\"Firmware erfolgreich installiert. OBI-C3 startet neu.\"}"
    );
    delay(800);
    ESP.restart();
}

void setup_ota_web() {
    server.on("/update", HTTP_GET, []() {
        server.send_P(200, "text/html; charset=utf-8", OTA_HTML);
    });

    server.on(
        "/api/firmware",
        HTTP_POST,
        handle_ota_result,
        handle_ota_upload
    );

    Serial.println("Web-OTA bereit: http://obi.local/update");
}

}  // namespace

void setup() {
    obi_base_setup();
    setup_ota_web();
}

void loop() {
    obi_base_loop();
}
