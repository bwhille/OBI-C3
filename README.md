# OBI-C3

![Firmware](https://img.shields.io/badge/Firmware-v0.4.4-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32--C3-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino%20%2F%20PlatformIO-green)

OBI-C3 macht aus einem **ESP32-C3 SuperMini** ein eigenständiges, per Browser bedienbares Diagnosegerät für kompatible Makita-LXT-Akkus.

Der ESP32-C3 übernimmt dabei gleichzeitig die zeitkritische Kommunikation mit dem Akku, die WLAN-Verbindung, den Webserver sowie die Aufbereitung der Akku- und Diagnosedaten. Ein Raspberry Pi oder zusätzlicher Arduino wird nicht benötigt.

> [!IMPORTANT]
> OBI-C3 ist ein unabhängiges Open-Source-Projekt. Es besteht keine geschäftliche, technische oder sonstige Verbindung zu Makita. Makita und LXT sind Marken ihrer jeweiligen Rechteinhaber.

Das Projekt basiert auf [mnh-jansson/open-battery-information](https://github.com/mnh-jansson/open-battery-information) und verwendet dessen angepasste OneWire-Implementierung.

## Aktueller Stand

- Aktuelle Firmware: **0.4.4**
- Standard-PlatformIO-Umgebung: `esp32-c3-web`
- Webzugriff im Heimnetz: `http://obi.local`
- OTA-Updates über GitHub Releases
- Lokaler Browser-Test ohne Hardware möglich
- ESP32-C3-Simulation mit Wokwi vorbereitet

## Funktionen

- Responsive Weboberfläche für Smartphone, Tablet und PC
- Umschaltbares helles und dunkles Design
- WLAN-Ersteinrichtung über einen eigenen Setup-Access-Point
- Speichern neuer WLAN-Zugangsdaten über die Weboberfläche
- Löschen der gespeicherten WLAN-Konfiguration für mobilen oder Baustellenbetrieb
- Zugriff im Heimnetz über mDNS unter `http://obi.local`
- Auslesen von Akku- und Diagnosedaten
- Anzeige von:
  - Akkumodell
  - Sperrstatus und Statuscode
  - Packspannung
  - einzelnen Zellspannungen
  - maximaler Zelldifferenz
  - Zelltemperatur
  - MOSFET-Temperatur
  - Ladezyklen
  - Kapazität
  - Herstellungsdatum
  - ROM-ID
- Diagnoseunterstützung für den Fehlercode F0513
- Schreibsperre für F0513
- Bestätigungspflichtiger Fehler-Reset durch Eingabe von `RESET`
- Automatische Suche nach neuen Firmwareversionen
- OTA-Installation direkt über die Weboberfläche
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

### 1. Repository klonen

```bash
git clone https://github.com/bwhille/OBI-C3.git
cd OBI-C3
```

### 2. Firmware kompilieren

```bash
pio run -e esp32-c3-web
```

### 3. ESP32-C3 flashen

```bash
pio run -e esp32-c3-web -t upload
```

In VS Code kann alternativ in der PlatformIO-Seitenleiste die Umgebung `esp32-c3-web` ausgewählt und anschließend **Upload** gestartet werden.

### 4. Seriellen Monitor öffnen

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

Im Heimnetz ist das Gerät normalerweise unter folgender Adresse erreichbar:

```text
http://obi.local
```

Falls mDNS im Netzwerk nicht funktioniert, kann die vom Router vergebene IP-Adresse verwendet werden. Sie wird außerdem im seriellen Monitor ausgegeben.

## WLAN-Konfiguration löschen

Die gespeicherten WLAN-Daten können direkt in der Weboberfläche über **WLAN löschen** entfernt werden.

Nach der Bestätigung startet der ESP32 neu und öffnet wieder den Setup-Hotspot. Das ist besonders praktisch, wenn das Gerät unterwegs, in einem anderen Netzwerk oder direkt über den eigenen Access Point betrieben werden soll.

## Bedienung

1. OBI-C3 über USB-C einschalten.
2. Akku korrekt in die Aufnahme einsetzen.
3. `http://obi.local` oder die IP-Adresse des Geräts öffnen.
4. **Akku auslesen** auswählen.
5. Messwerte und Diagnoseinformationen prüfen.
6. Vor einem Fehler-Reset Zellen, Temperatursensoren, Kontakte, Verdrahtung und BMS kontrollieren.
7. Zum Löschen eines unterstützten Fehlers ausdrücklich `RESET` eingeben und den Vorgang bestätigen.

## OTA-Firmwareupdates

OBI-C3 kann neue Firmwareversionen direkt über die Weboberfläche installieren.

### Update auf dem Gerät installieren

1. OBI-C3 mit einem WLAN mit Internetzugang verbinden.
2. Die Weboberfläche öffnen.
3. **Nach Updates suchen** auswählen.
4. Eine gefundene neue Version prüfen.
5. **Jetzt aktualisieren** bestätigen.
6. Das Gerät während des Updates nicht ausschalten.

Der ESP32 lädt die neue `firmware.bin`, installiert sie und startet anschließend automatisch neu.

> [!CAUTION]
> Während eines OTA-Updates sollte kein Akku eingesetzt sein. OBI-C3 muss über eine stabile USB-C-Stromversorgung versorgt werden. Ein Stromausfall während des Flashens kann dazu führen, dass die Firmware anschließend per USB neu eingespielt werden muss.

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

GitHub Actions kompiliert daraus automatisch die Firmware und erstellt ein GitHub Release mit der Datei `firmware.bin`.

Die Versionsnummer des Tags und die Werte in `src/main.cpp` sollten immer übereinstimmen.

## Virtuell testen

### Lokaler Browser-Test

Der Mock-Server verwendet die in der Firmware eingebettete Weboberfläche und simuliert einen BL1850B-Akku. Benötigt wird lediglich Python 3.

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

Firmware kompilieren:

```bash
pio run -e esp32-c3-simulation
```

Danach kann die Simulation über die VS-Code-Erweiterung **Wokwi Simulator** gestartet werden. Die Simulationsfirmware verbindet sich automatisch mit `Wokwi-GUEST` und verwendet einen virtuellen Akku.

Für den direkten Browserzugriff auf den simulierten Webserver wird das Wokwi Private IoT Gateway benötigt. Mit dessen Standardweiterleitung ist die Oberfläche normalerweise hier erreichbar:

```text
http://localhost:9080
```

Ohne Private Gateway können weiterhin Firmwarestart, WLAN-Verbindung und serielle Ausgaben geprüft werden. Für den vollständigen kostenlosen Browser-Test steht der lokale Mock-Server zur Verfügung.

## PlatformIO-Umgebungen

| Umgebung | Verwendung |
| --- | --- |
| `esp32-c3-web` | Standardfirmware mit WLAN und Weboberfläche |
| `esp32-c3-simulation` | Webfirmware mit simuliertem Akku für Wokwi |
| `esp32-c3-devkitm-1` | Serielle ESP32-C3-OBI-Firmware ohne Weboberfläche |
| `uno` | Ursprüngliche serielle Arduino-Uno-Variante |
| `nano` | Ursprüngliche serielle Arduino-Nano-Variante |

## Fehlerbehebung

### `obi.local` ist nicht erreichbar

- Prüfen, ob das Gerät mit dem richtigen WLAN verbunden ist.
- Sicherstellen, dass sich Client und OBI-C3 im selben Netzwerk befinden.
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
- Sicherstellen, dass das Release eine Datei mit dem Namen `firmware.bin` enthält.
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
