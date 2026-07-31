# OBI-C3

![Firmware](https://img.shields.io/badge/Firmware-v0.4.4-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32--C3-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino%20%2F%20PlatformIO-green)

OBI-C3 macht aus einem **ESP32-C3 SuperMini** ein eigenständiges, per Browser bedienbares Diagnosegerät für kompatible Makita-LXT-Akkus.

Der ESP32-C3 übernimmt die zeitkritische Kommunikation mit dem Akku, die WLAN-Verbindung, den Webserver sowie die Aufbereitung der Akku- und Diagnosedaten. Ein Raspberry Pi oder zusätzlicher Arduino wird nicht benötigt.

> [!IMPORTANT]
> OBI-C3 ist ein unabhängiges Open-Source-Projekt. Es besteht keine geschäftliche, technische oder sonstige Verbindung zu Makita. Makita und LXT sind Marken ihrer jeweiligen Rechteinhaber.

Das Projekt basiert auf [mnh-jansson/open-battery-information](https://github.com/mnh-jansson/open-battery-information) und verwendet dessen angepasste OneWire-Implementierung.

## Aktueller Stand

- Aktuelle Firmware: **0.4.4**
- Standard-PlatformIO-Umgebung: `esp32-c3-web`
- Browserbasierte Diagnoseoberfläche
- Automatische OTA-Updates über GitHub Releases
- Lokaler Browser-Test ohne Hardware möglich

## Funktionen

- Responsive Weboberfläche für Smartphone, Tablet und PC
- Umschaltbares helles und dunkles Design
- WLAN-Ersteinrichtung über einen eigenen Setup-Access-Point
- Speichern neuer WLAN-Zugangsdaten über die Weboberfläche
- Löschen der gespeicherten WLAN-Konfiguration
- Auslesen von Akku- und Diagnosedaten
- Anzeige von:
  - Akkumodell
  - Sperrstatus und Statuscode
  - Packspannung
  - einzelnen Zellspannungen
  - maximaler Zelldifferenz
  - Zelltemperatur
  - MOSFET-Temperatur, sofern vom Akku unterstützt
  - Ladezyklen
  - Kapazität
  - Herstellungsdatum
  - ROM-ID
- Diagnoseunterstützung für den Fehlercode F0513
- Gesperrter Fehler-Reset bei F0513
- Bestätigungspflichtiger Fehler-Reset durch Eingabe von `RESET`
- Automatische Suche nach neuen Firmwareversionen
- OTA-Installation direkt aus einem GitHub Release
- Serielle OBI-Funktion über die übrigen PlatformIO-Umgebungen

Der im Ursprungsprojekt nicht implementierte **Battery message reset** ist nicht Bestandteil dieser Firmware.

## Benötigte Hardware

- ESP32-C3 SuperMini mit USB-C
- OBI-Schaltung gemäß dem Originalprojekt
- externes und stabiles 5-V-USB-C-Netzteil
- geeignete Akkuaufnahme für kompatible LXT-Akkus
- passende Widerstände und Verdrahtung für die OBI-Schnittstelle

### Pinbelegung

| Funktion | ESP32-C3-Anschluss |
| --- | --- |
| OBI Enable | GPIO 0 |
| OBI OneWire | GPIO 1 |
| Versorgung | USB-C, 5 V |
| Pull-up-Spannung | 3,3 V |

> [!WARNING]
> Der ESP32-C3 ist nicht 5-V-tolerant. Die Pull-up-Widerstände der OBI-Schnittstelle müssen deshalb an **3,3 V** angeschlossen werden.

Der zu prüfende Akku versorgt weder den ESP32 noch den Webserver. Das Gerät bleibt dadurch auch bei gesperrten oder tiefentladenen Akkus erreichbar.

## Schnellstart

### Repository klonen

```bash
git clone https://github.com/bwhille/OBI-C3.git
cd OBI-C3
```

### Firmware kompilieren

```bash
pio run -e esp32-c3-web
```

### ESP32-C3 flashen

```bash
pio run -e esp32-c3-web -t upload
```

In VS Code kann alternativ in der PlatformIO-Seitenleiste die Umgebung `esp32-c3-web` ausgewählt und anschließend **Upload** gestartet werden.

### Seriellen Monitor öffnen

```bash
pio device monitor -b 115200
```

## Erste WLAN-Einrichtung

Ist noch kein WLAN gespeichert oder kann die gespeicherte Verbindung nicht aufgebaut werden, startet der ESP32 einen eigenen Setup-Access-Point:

```text
WLAN: OBI-Setup-XXXXXX
Passwort: obi-setup
Adresse: http://192.168.4.1
```

1. Mit dem WLAN `OBI-Setup-XXXXXX` verbinden.
2. `http://192.168.4.1` im Browser öffnen.
3. Das gewünschte 2,4-GHz-WLAN und dessen Passwort eintragen.
4. Die Einstellungen speichern.
5. Der ESP32 startet anschließend neu und verbindet sich mit dem gewählten WLAN.

Nach der Verbindung wird die Weboberfläche über die vom Router vergebene IP-Adresse geöffnet:

```text
http://<IP-DES-OBI-C3>/
```

Die IP-Adresse kann im Router oder im seriellen Monitor ermittelt werden.

## WLAN-Konfiguration löschen

Die gespeicherten WLAN-Daten können direkt in der Weboberfläche über **WLAN löschen** entfernt werden.

Nach der Bestätigung startet der ESP32 neu und öffnet wieder den Setup-Hotspot. Dies ist für einen Netzwerkwechsel oder den direkten Betrieb über den Setup-Access-Point vorgesehen.

## Bedienung

1. OBI-C3 über USB-C einschalten.
2. Akku korrekt in die Aufnahme einsetzen.
3. Die IP-Adresse des Geräts im Browser öffnen.
4. **Akku auslesen** auswählen.
5. Messwerte und Diagnoseinformationen prüfen.
6. Vor einem Fehler-Reset Zellen, Temperatursensoren, Kontakte, Verdrahtung und BMS kontrollieren.
7. Zum Löschen eines unterstützten Fehlers ausdrücklich `RESET` eingeben und den Vorgang bestätigen.

## OTA-Firmwareupdates

OBI-C3 kann neue Firmwareversionen direkt über die normale Weboberfläche installieren. Eine separate Seite unter `/update` und ein manueller Upload einer lokalen Firmwaredatei sind in der aktuellen Firmware nicht vorgesehen.

### Update auf dem Gerät installieren

1. Akku aus der Aufnahme entfernen.
2. OBI-C3 mit einem WLAN mit Internetzugang verbinden.
3. Die Weboberfläche über die IP-Adresse öffnen.
4. Im Bereich **Firmware-Update** auf **Nach Updates suchen** klicken.
5. Eine gefundene neue Version prüfen.
6. **Jetzt aktualisieren** auswählen.
7. Das Gerät während Download und Installation nicht ausschalten.

Der ESP32 lädt die Datei `firmware.bin` aus dem neuesten GitHub Release, installiert sie und startet anschließend automatisch neu.

> [!CAUTION]
> Während eines OTA-Updates sollte kein Akku eingesetzt sein. OBI-C3 muss über eine stabile USB-C-Stromversorgung versorgt werden. Ein Stromausfall während des Flashens kann dazu führen, dass die Firmware anschließend per USB neu eingespielt werden muss.

Eine ausführliche Beschreibung befindet sich in [docs/WEB_OTA.md](docs/WEB_OTA.md).

### Neues Release veröffentlichen

1. Versionsnummer in `src/main.cpp` erhöhen.
2. Änderungen committen und nach `main` pushen.
3. Einen Git-Tag mit derselben Versionsnummer erstellen.
4. Den Tag zu GitHub übertragen.

Beispiel:

```bash
git add .
git commit -m "Release v0.4.5"
git push origin main
git tag v0.4.5
git push origin v0.4.5
```

GitHub Actions kompiliert die Umgebung `esp32-c3-web` und erstellt beziehungsweise aktualisiert ein GitHub Release mit der Datei `firmware.bin`.

Die Versionsnummer des Tags und die Werte in `src/main.cpp` müssen übereinstimmen.

## Virtuell testen

### Lokaler Browser-Test

Der Mock-Server verwendet die in der Firmware eingebettete Weboberfläche und simuliert einen BL1850B-Akku. Benötigt wird Python 3.

```bash
python3 tools/mock_server.py
```

Danach im Browser öffnen:

```text
http://127.0.0.1:8080
```

Der simulierte Akku startet unter anderem mit:

- Zustand `LOCKED`
- Statuscode `04`
- fünf simulierten Zellspannungen
- simulierten Temperaturen
- simulierten Ladezyklen

Nach Eingabe von `RESET` wechselt der virtuelle Akku zu `UNLOCKED` und zum Statuscode `00`. Beim erneuten Auslesen verändern sich die simulierten Messwerte geringfügig.

### Wokwi-Simulation

Das Repository enthält die Dateien `diagram.json`, `wokwi.toml` sowie die PlatformIO-Umgebung `esp32-c3-simulation`.

```bash
pio run -e esp32-c3-simulation
```

Die Simulation kann anschließend über die VS-Code-Erweiterung **Wokwi Simulator** gestartet werden. Die Simulationsfirmware verbindet sich automatisch mit `Wokwi-GUEST` und verwendet einen virtuellen Akku.

Für einen direkten Browserzugriff auf den simulierten Webserver ist ein Wokwi Private IoT Gateway erforderlich. Ohne Gateway können Firmwarestart, WLAN-Verbindung und serielle Ausgaben geprüft werden. Für einen vollständigen Browser-Test steht der lokale Mock-Server zur Verfügung.

## PlatformIO-Umgebungen

| Umgebung | Verwendung |
| --- | --- |
| `esp32-c3-web` | Standardfirmware mit WLAN und Weboberfläche |
| `esp32-c3-simulation` | Webfirmware mit simuliertem Akku für Wokwi |
| `esp32-c3-devkitm-1` | Serielle ESP32-C3-OBI-Firmware ohne Weboberfläche |
| `uno` | Ursprüngliche serielle Arduino-Uno-Variante |
| `nano` | Ursprüngliche serielle Arduino-Nano-Variante |

## Fehlerbehebung

### Weboberfläche ist nicht erreichbar

- Prüfen, ob das Gerät mit dem richtigen WLAN verbunden ist.
- Sicherstellen, dass sich Client und OBI-C3 im selben erreichbaren Netzwerk befinden.
- Die IP-Adresse im Router oder seriellen Monitor ermitteln.
- Gastnetz-Isolation, Client-Isolation oder VLAN-Firewallregeln prüfen.

### Kein Akku erkannt

- Akkuaufnahme und Kontaktierung kontrollieren.
- GPIO 0 und GPIO 1 prüfen.
- Pull-up-Widerstände auf 3,3 V kontrollieren.
- Gemeinsame Masse und externe USB-C-Stromversorgung prüfen.
- Akku vollständig einsetzen und erneut auslesen.

### OTA-Update wird nicht gefunden

- Internetzugang des ESP32 prüfen.
- Kontrollieren, ob ein GitHub Release mit passendem Versions-Tag existiert.
- Sicherstellen, dass das Release eine Datei mit dem exakten Namen `firmware.bin` enthält.
- Prüfen, ob die veröffentlichte Version höher als die installierte Version ist.

## Sicherheit

Das Löschen eines Fehlercodes repariert keine defekte Zelle, keinen Temperatursensor, keine beschädigte Leiterbahn und kein defektes BMS.

Beschädigte, mechanisch verformte, korrodierte, heiße, aufgeblähte, tiefentladene oder stark unausgeglichene Akkus dürfen nicht zurückgesetzt und anschließend unbeaufsichtigt geladen oder verwendet werden.

Arbeiten an Lithium-Ionen-Akkus können Brand-, Explosions-, Stromschlag- und Verletzungsgefahren verursachen. Diagnose- und Resetfunktionen dürfen nur von Personen eingesetzt werden, die Zustand und Risiken des Akkupacks fachlich beurteilen können.

## Haftungsausschluss

Die Software und die zugehörigen Informationen werden ohne Gewährleistung bereitgestellt. Die Nutzung, der Aufbau der Hardware, die Diagnose von Akkus sowie das Löschen von Fehlercodes erfolgen ausschließlich auf eigene Verantwortung.

Die Projektverantwortlichen übernehmen keine Haftung für Schäden an Akkus, Ladegeräten, Werkzeugen, elektronischen Bauteilen oder sonstigem Eigentum sowie für Personen- und Folgeschäden, soweit dies gesetzlich zulässig ist.

## Lizenz und Herkunft

Dieses Projekt basiert auf [mnh-jansson/open-battery-information](https://github.com/mnh-jansson/open-battery-information). Die jeweiligen Lizenzbedingungen des Ursprungsprojekts und dieses Repositories sind zu beachten.

Marken- und Produktnamen werden ausschließlich zur Beschreibung der technischen Kompatibilität verwendet. Es handelt sich nicht um ein Produkt von Makita und nicht um ein von Makita geprüftes, autorisiertes oder unterstütztes Zubehör.
