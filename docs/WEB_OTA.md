# Firmware-Update über die OBI-C3-Weboberfläche

OBI-C3 unterstützt automatische Firmware-Updates direkt über die normale Weboberfläche. Eine separate Update-Seite unter `/update` und das manuelle Hochladen einer lokalen `firmware.bin` werden von der aktuellen Firmware nicht mehr verwendet.

## Voraussetzungen

- OBI-C3 läuft mit einer OTA-fähigen Firmware.
- Das Gerät ist mit einem 2,4-GHz-WLAN verbunden.
- Das WLAN verfügt über Internetzugang.
- Ein GitHub Release mit einer höheren Versionsnummer ist vorhanden.
- Das Release enthält eine Datei mit dem exakten Namen `firmware.bin`.

Die Weboberfläche wird über die vom Router zugewiesene IP-Adresse geöffnet:

```text
http://<IP-DES-OBI-C3>/
```

Die IP-Adresse kann im Router oder im seriellen Monitor ermittelt werden.

## Update durchführen

1. Akku aus der OBI-C3-Aufnahme entfernen.
2. ESP32-C3 stabil über USB-C mit Strom versorgen.
3. Die Weboberfläche über die IP-Adresse öffnen.
4. Im Bereich **Firmware-Update** auf **Nach Updates suchen** klicken.
5. Prüfen, ob eine neuere Firmwareversion gefunden wurde.
6. Auf **Jetzt aktualisieren** klicken.
7. Die Stromversorgung während Download und Installation nicht trennen.
8. Warten, bis OBI-C3 die Firmware installiert hat und automatisch neu startet.
9. Die Weboberfläche anschließend erneut über die IP-Adresse öffnen.

Der ESP32 lädt die Datei `firmware.bin` selbstständig aus dem neuesten GitHub Release herunter. Eine lokale Firmwaredatei muss im Browser nicht ausgewählt werden.

## Neues Firmware-Release veröffentlichen

### 1. Versionsnummer erhöhen

Die Firmwareversion wird in `src/main.cpp` festgelegt:

```cpp
#define ARDUINO_OBI_VERSION_MAJOR 0
#define ARDUINO_OBI_VERSION_MINOR 4
#define ARDUINO_OBI_VERSION_PATCH 4
```

Vor einem neuen Release muss mindestens einer dieser Werte erhöht werden.

### 2. Änderungen nach `main` übertragen

```bash
git add .
git commit -m "Release v0.4.5"
git push origin main
```

### 3. Release-Tag erstellen

```bash
git tag v0.4.5
git push origin v0.4.5
```

Der Release-Workflow akzeptiert Tags mit vorangestelltem `v`, beispielsweise `v0.4.5`, sowie rein numerische Tags wie `0.4.5`. Empfohlen wird einheitlich das Format `vMAJOR.MINOR.PATCH`.

GitHub Actions kompiliert die PlatformIO-Umgebung `esp32-c3-web` und erstellt beziehungsweise aktualisiert ein GitHub Release. Die erzeugte Datei wird als `firmware.bin` an das Release angehängt.

Die Versionsnummer in `src/main.cpp` und die Versionsnummer des Git-Tags müssen übereinstimmen. Andernfalls kann die Weboberfläche einen falschen Update-Status anzeigen.

## Technischer Ablauf

Die Weboberfläche verwendet folgende interne API-Endpunkte:

```text
GET  /api/update/check
POST /api/update/install
```

`/api/update/check` liest das neueste Release aus dem GitHub-Repository aus und vergleicht dessen Tag mit der installierten Firmwareversion.

`/api/update/install` lädt die im Release enthaltene `firmware.bin`, schreibt sie über die ESP32-Update-Funktion in die OTA-Partition und startet das Gerät nach erfolgreicher Installation neu.

Diese API-Endpunkte sind für die Weboberfläche vorgesehen und müssen bei normaler Bedienung nicht manuell aufgerufen werden.

## Sicherheitsverhalten

- Während des Updates sollte kein Akku eingesetzt sein.
- OBI-C3 muss stabil über USB-C versorgt werden.
- Die Firmware wird erst nach einem vollständig übertragenen Download aktiviert.
- Bei einem unvollständigen oder fehlgeschlagenen Download wird die Installation abgebrochen.
- Ein Stromausfall während des Schreibvorgangs kann eine erneute Installation per USB erforderlich machen.

## Fehlerbehebung

### Es wird kein Update gefunden

- Prüfen, ob der ESP32 mit einem WLAN mit Internetzugang verbunden ist.
- Prüfen, ob ein GitHub Release vorhanden ist.
- Prüfen, ob das neueste Release die Datei `firmware.bin` enthält.
- Prüfen, ob die Release-Version tatsächlich höher als die installierte Version ist.
- Prüfen, ob der Tag als gültige dreiteilige Versionsnummer angelegt wurde, zum Beispiel `v0.4.5`.

### Meldung „Noch kein Firmware-Release veröffentlicht“

Für das Repository ist noch kein über die GitHub-API abrufbares Release vorhanden. Ein Git-Tag allein reicht nicht aus, falls der Release-Workflow nicht erfolgreich gelaufen ist.

### Update schlägt während des Downloads fehl

- Internetverbindung des ESP32 prüfen.
- Stromversorgung kontrollieren.
- Sicherstellen, dass `firmware.bin` vollständig im neuesten Release vorhanden ist.
- Bei einer nicht mehr startenden Firmware OBI-C3 per USB neu flashen:

```bash
pio run -e esp32-c3-web -t upload
```

## Gespeicherte Einstellungen

Ein normales OTA-Update ersetzt die Anwendungsfirmware. Die in den ESP32-Preferences gespeicherten WLAN-Zugangsdaten bleiben dabei normalerweise erhalten.
