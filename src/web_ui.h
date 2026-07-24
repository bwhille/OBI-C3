#pragma once

const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>OBI Web</title>
  <style>
    :root{color-scheme:dark;--bg:#0b0e11;--panel:#171c22;--line:#2a333d;
      --text:#f4f7f9;--muted:#9ba8b4;--yellow:#ffb000;--green:#42c983;
      --red:#ff5b5b;font-family:system-ui,-apple-system,"Segoe UI",sans-serif}
    *{box-sizing:border-box}body{margin:0;background:radial-gradient(circle at
      10% 0%,#ffb00015,transparent 28rem),var(--bg);color:var(--text)}
    header,main{width:min(1080px,calc(100% - 28px));margin:auto}
    header{display:flex;align-items:center;justify-content:space-between;
      padding:30px 0 20px}.eyebrow{color:var(--yellow);font-size:.75rem;
      font-weight:800;letter-spacing:.13em;text-transform:uppercase;margin:0 0 3px}
    h1{font-size:clamp(1.6rem,5vw,2.35rem);margin:0}h2{font-size:1rem;margin:0 0 15px}
    .panel{background:linear-gradient(145deg,#1c232b,var(--panel));border:1px solid
      var(--line);border-radius:15px;box-shadow:0 14px 35px #0003}
    .badge{display:flex;align-items:center;gap:8px;padding:8px 12px;border:1px solid
      var(--line);border-radius:99px;color:var(--muted);font-size:.82rem}
    .dot{width:8px;height:8px;border-radius:50%;background:var(--red)}
    .online .dot{background:var(--green)}.controls{display:flex;gap:10px;
      padding:16px;flex-wrap:wrap}button,input{min-height:42px;border-radius:9px;
      border:1px solid var(--line);padding:0 14px;font:inherit}
    button{font-weight:750;cursor:pointer;background:#252e38;color:var(--text)}
    button.primary{background:var(--yellow);color:#17120a;border:0}
    button.danger{background:#b93636;border:0}button:disabled{opacity:.4;cursor:not-allowed}
    .notice{margin-top:13px;padding:12px 14px;border:1px solid var(--line);
      border-radius:9px;background:var(--panel)}.notice.ok{border-color:#42c98388}
    .notice.error{border-color:#ff5b5baa;color:#ffb2b2}.hidden{display:none}
    .summary{display:grid;grid-template-columns:repeat(4,1fr);gap:12px;margin-top:14px}
    .metric{padding:18px;display:grid;gap:8px}.metric span,dt,.muted,label{
      color:var(--muted);font-size:.8rem}.metric strong{font-size:1.35rem}
    .content{display:grid;grid-template-columns:.85fr 1.15fr;gap:12px;margin-top:12px}
    .content>.panel,.danger,.wifi{padding:20px}.cells{display:grid;gap:5px}
    .cells div,.details div{display:flex;justify-content:space-between;gap:12px;
      border-bottom:1px solid var(--line);padding:10px 1px}.cells strong{color:#ffc84f}
    dl{margin:0}dd{margin:0;text-align:right}.mono{font-family:monospace;
      font-size:.75rem;word-break:break-all}.danger,.wifi{display:flex;
      align-items:center;justify-content:space-between;gap:20px;margin-top:12px}
    .danger{border-color:#ff5b5b66}.danger p,.wifi p{color:var(--muted);
      line-height:1.45;margin:5px 0 0}.wifi-form{display:flex;gap:8px;flex-wrap:wrap}
    dialog{width:min(480px,calc(100% - 26px));border:1px solid var(--line);
      border-radius:14px;background:var(--panel);color:var(--text);padding:22px}
    dialog::backdrop{background:#000b}dialog input{width:100%;background:#0e1216;
      color:var(--text);margin:8px 0 16px}.actions{display:flex;gap:8px;justify-content:flex-end}
    @media(max-width:760px){.summary{grid-template-columns:1fr 1fr}.content{
      grid-template-columns:1fr}.danger,.wifi{align-items:stretch;flex-direction:column}}
  </style>
</head>
<body>
  <header>
    <div><p class="eyebrow">Open Battery Information</p><h1>Makita LXT Diagnose</h1></div>
    <div id="badge" class="badge"><span class="dot"></span><span>Initialisiere</span></div>
  </header>
  <main>
    <section class="panel controls">
      <button id="read" class="primary">Akku auslesen</button>
      <button id="refresh" disabled>Messwerte aktualisieren</button>
      <button id="reload">Status aktualisieren</button>
    </section>
    <div id="notice" class="notice hidden"></div>
    <section class="summary">
      <article class="panel metric"><span>Modell</span><strong id="model">–</strong></article>
      <article class="panel metric"><span>Zustand</span><strong id="state">–</strong></article>
      <article class="panel metric"><span>Packspannung</span><strong id="pack">–</strong></article>
      <article class="panel metric"><span>Zelldifferenz</span><strong id="difference">–</strong></article>
    </section>
    <section class="content">
      <article class="panel">
        <h2>Zellspannungen</h2>
        <div id="cells" class="cells">
          <div><span>Zelle 1</span><strong>–</strong></div>
          <div><span>Zelle 2</span><strong>–</strong></div>
          <div><span>Zelle 3</span><strong>–</strong></div>
          <div><span>Zelle 4</span><strong>–</strong></div>
          <div><span>Zelle 5</span><strong>–</strong></div>
        </div>
      </article>
      <article class="panel">
        <h2>Akkuinformationen</h2>
        <dl class="details">
          <div><dt>Statuscode</dt><dd id="code">–</dd></div>
          <div><dt>Ladezyklen</dt><dd id="cycles">–</dd></div>
          <div><dt>Kapazität</dt><dd id="capacity">–</dd></div>
          <div><dt>Herstellungsdatum</dt><dd id="date">–</dd></div>
          <div><dt>Zelltemperatur</dt><dd id="tempCell">–</dd></div>
          <div><dt>MOSFET-Temperatur</dt><dd id="tempMosfet">–</dd></div>
          <div><dt>ROM-ID</dt><dd id="rom" class="mono">–</dd></div>
          <div><dt>Firmware</dt><dd id="firmware">–</dd></div>
          <div><dt>Adresse</dt><dd id="ip">–</dd></div>
        </dl>
      </article>
    </section>
    <section class="panel danger">
      <div><h2>Fehler zurücksetzen</h2><p>Nur nach Prüfung und Reparatur der
        technischen Ursache ausführen. F0513 wird nur diagnostisch unterstützt.</p></div>
      <button id="reset" class="danger" disabled>Fehler löschen</button>
    </section>
    <section class="panel wifi">
      <div><h2>WLAN-Einrichtung</h2><p id="wifiState">–</p></div>
      <form id="wifiForm" class="wifi-form">
        <input name="ssid" placeholder="2,4-GHz-WLAN" required>
        <input name="password" type="password" placeholder="WLAN-Passwort">
        <button>Speichern</button>
      </form>
    </section>
  </main>
  <dialog id="dialog">
    <form method="dialog">
      <h2>Fehler wirklich löschen?</h2>
      <p class="muted">Das Löschen repariert keinen defekten Akku. Zum
        Bestätigen <strong>RESET</strong> eingeben.</p>
      <input id="confirmation" autocomplete="off">
      <div class="actions"><button value="cancel">Abbrechen</button>
        <button id="confirm" value="default" class="danger">Jetzt löschen</button></div>
    </form>
  </dialog>
  <script>
    const $=s=>document.querySelector(s), els={
      badge:$("#badge"),notice:$("#notice"),read:$("#read"),refresh:$("#refresh"),
      reset:$("#reset"),dialog:$("#dialog"),confirmation:$("#confirmation")};
    let battery=null;
    const text=(id,value,suffix="")=>{$("#"+id).textContent=
      value===null||value===undefined||value===""?"–":value+suffix};
    const number=(value,digits)=>value===null||value===undefined?null:
      new Intl.NumberFormat("de-DE",{minimumFractionDigits:digits,
      maximumFractionDigits:digits}).format(value);
    function notice(message,type="ok"){els.notice.textContent=message;
      els.notice.className="notice "+type}
    async function api(path,options={}){const response=await fetch(path,options);
      const body=await response.json().catch(()=>({}));if(!response.ok)
      throw new Error(body.error||"HTTP "+response.status);return body}
    function renderStatus(status){text("firmware",status.firmware_version);
      text("ip",status.ip);$("#wifiState").textContent=(status.wifi_mode==="ap"?
      "Einrichtungsmodus: ":"Verbunden mit ")+status.ssid+" · "+status.ip;
      els.badge.className="badge online";els.badge.lastElementChild.textContent=
      status.wifi_mode==="ap"?"Setup-WLAN":"Online"}
    function renderBattery(data){battery=data;if(!data||!data.valid)return;
      text("model",data.model);text("state",data.state);text("pack",
      number(data.pack_voltage,3)," V");text("difference",
      number(data.cell_voltage_difference,3)," V");text("code",data.status_code);
      text("cycles",data.charge_count);text("capacity",number(data.capacity_ah,1)," Ah");
      text("date",data.manufacturing_date);text("tempCell",
      number(data.temperature_cell,1)," °C");text("tempMosfet",
      number(data.temperature_mosfet,1)," °C");text("rom",data.rom_id);
      const cells=document.querySelectorAll("#cells strong");
      (data.cell_voltages||[]).forEach((v,i)=>cells[i].textContent=number(v,3)+" V");
      els.refresh.disabled=false;els.reset.disabled=!!data.diagnostics_only}
    async function load(){try{const data=await api("/api/status");
      renderStatus(data.status);renderBattery(data.battery)}catch(e){notice(e.message,"error")}}
    async function run(path){document.body.style.cursor="progress";try{
      const result=await api(path,{method:"POST"});
      renderBattery(result.battery);notice("Akku erfolgreich ausgelesen.")}
      catch(e){notice(e.message,"error")}finally{document.body.style.cursor=""}}
    els.read.onclick=()=>run("/api/read");els.refresh.onclick=()=>run("/api/read/live");
    $("#reload").onclick=load;els.reset.onclick=()=>{els.confirmation.value="";
      els.dialog.showModal()};
    $("#confirm").onclick=async event=>{event.preventDefault();
      if(els.confirmation.value!=="RESET"){notice("Bitte RESET exakt eingeben.","error");
      return}els.dialog.close();const form=new URLSearchParams({confirmation:"RESET"});
      try{const result=await api("/api/errors/reset",{method:"POST",
      headers:{"Content-Type":"application/x-www-form-urlencoded"},body:form});
      renderBattery(result.battery);notice(result.message+" Vorher: "+
      result.before.state+"/"+result.before.status_code+", nachher: "+
      result.after.state+"/"+result.after.status_code+".")}catch(e){notice(e.message,"error")}};
    $("#wifiForm").onsubmit=async event=>{event.preventDefault();
      try{const body=new URLSearchParams(new FormData(event.target));
      const result=await api("/api/wifi",{method:"POST",headers:
      {"Content-Type":"application/x-www-form-urlencoded"},body});
      notice(result.message)}catch(e){notice(e.message,"error")}};
    load();
  </script>
</body>
</html>
)HTML";
