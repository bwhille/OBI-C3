# OBI-C3

OBI-C3 macht aus einem ESP32-C3 SuperMini ein eigenständiges, per Browser
bedienbares Open-Battery-Information-Gerät für Makita-LXT-Akkus.

Der ESP32-C3 übernimmt gleichzeitig:

- die zeitkritische Kommunikation mit dem Akku,
- den Webserver und die WLAN-Verbindung,
- das Auslesen von Akku- und Diagnosedaten,
- das bestätigungspflichtige Löschen unterstützter Fehler.

Ein Raspberry Pi oder zusätzlicher Arduino wird nicht benötigt.

Das Projekt basiert auf
[mnh-jansson/open-battery-information](https://github.com/mnh-jansson/open-battery-information)
und verwendet dessen angepasste OneWire-Implementierung.

## Funktionen

- Responsive Weboberfläche für Handy, Tablet und PC
- WLAN-Ersteinrichtung über einen eigenen Access Point
- Zugriff im Heimnetz über `http://obi.local`
- Anzeige von:
  - Akkumodell
  - Sperrstatus und Statuscode
  - Pack- und Zellspannungen
  - maximaler Zelldifferenz
  - Zell- und MOSFET-Temperatur
  - Ladezyklen
  - Kapazität und Herstellungsdatum
  - ROM-ID
- Fehler-Reset nur nach Eingabe von `RESET`
- Diagnoseunterstützung für F0513
- Schreibsperre für F0513
- ursprüngliche serielle OBI-Funktion bleibt über die anderen
  PlatformIO-Umgebungen erhalten

Der im Ursprungsprojekt noch nicht implementierte „Battery message reset“ ist
nicht enthalten.

## Hardware

- ESP32-C3 SuperMini mit USB-C
- OBI-Schaltung gemäß dem Originalprojekt
- externes 5-V-USB-C-Netzteil
- passende Makita-LXT-Akkuaufnahme

| Funktion | ESP32-C3-Anschluss |
| --- | --- |
| OBI Enable | GPIO 0 |
| OBI OneWire | GPIO 1 |
| Versorgung | USB-C, 5 V |
| Pull-up-Spannung | 3,3 V |

Der ESP32-C3 ist nicht 5-V-tolerant. Die Pull-up-Widerstände der
OBI-Schnittstelle müssen deshalb an 3,3 V angeschlossen werden.

Der zu prüfende Akku versorgt weder den ESP32 noch den Webserver. Dadurch bleibt
das Gerät auch bei gesperrten oder tiefentladenen Akkus erreichbar.

## Kompilieren und flashen

Benötigt werden VS Code, PlatformIO und ein per USB angeschlossener ESP32-C3.

```bash
git clone https://github.com/bwhille/OBI-C3.git
cd OBI-C3
pio run -e esp32-c3-web -t upload
```

In VS Code kann alternativ in der PlatformIO-Seitenleiste die Umgebung
`esp32-c3-web` ausgewählt und anschließend `Upload` ausgeführt werden.

Der serielle Monitor arbeitet mit 115200 Baud:

```bash
pio device monitor -b 115200
```

## Erste WLAN-Einrichtung

Ist noch kein WLAN gespeichert oder kann die gespeicherte Verbindung nicht
aufgebaut werden, startet der ESP32 folgenden Access Point:

```text
WLAN: OBI-Setup-XXXXXX
Passwort: obi-setup
Adresse: http://192.168.4.1
```

Auf der Webseite das gewünschte 2,4-GHz-WLAN und dessen Passwort eintragen.
Anschließend startet der ESP32 neu.

Im Heimnetz ist das Gerät normalerweise erreichbar unter:

```text
http://obi.local
```

Wenn mDNS im Netzwerk nicht funktioniert, zeigt der serielle Monitor die
vergebene IP-Adresse an.

## Virtuell testen

### Kostenloser Browser-Test

Der lokale Mock-Server verwendet exakt die in der Firmware eingebettete
Weboberfläche und simuliert einen BL1850B-Akku. Dafür ist nur Python 3 nötig:

```bash
python3 tools/mock_server.py
```

Danach im Browser öffnen:

```text
http://127.0.0.1:8080
```

Der simulierte Akku startet mit:

- Zustand `LOCKED`
- Statuscode `04`
- fünf realistischen Zellspannungen
- simulierten Temperaturen und Ladezyklen

Nach Eingabe von `RESET` wechselt der virtuelle Akku zu `UNLOCKED` und
Statuscode `00`. Beim Aktualisieren verändern sich die Messwerte geringfügig.

### ESP32-C3 mit Wokwi simulieren

Das Repository enthält `diagram.json`, `wokwi.toml` und die
PlatformIO-Umgebung `esp32-c3-simulation`.

Zuerst die Simulationsfirmware kompilieren:

```bash
pio run -e esp32-c3-simulation
```

Anschließend in VS Code die Erweiterung „Wokwi Simulator“ öffnen und die
Simulation starten. Die Firmware verbindet sich automatisch mit
`Wokwi-GUEST` und verwendet denselben virtuellen Akku wie der lokale
Mock-Server.

Für den direkten Zugriff auf den simulierten ESP32-Webserver aus dem Browser
wird das Wokwi Private IoT Gateway benötigt. Mit dessen Standardweiterleitung
ist die Webseite unter folgender Adresse erreichbar:

```text
http://localhost:9080
```

Ohne Private Gateway lassen sich Firmwarestart, WLAN-Verbindung und Ausgaben im
seriellen Monitor testen. Der lokale Mock-Server bleibt für die vollständige
Browserbedienung kostenlos nutzbar.

## Bedienung

1. ESP32-C3 über USB-C einschalten.
2. Akku in die OBI-Aufnahme einsetzen.
3. `http://obi.local` öffnen.
4. `Akku auslesen` auswählen.
5. Vor einem Fehler-Reset Zellen, Sensoren, Anschlüsse und BMS prüfen.
6. Zum Löschen auf der Webseite ausdrücklich `RESET` eingeben.

## Sicherheit

Das Löschen eines Fehlers repariert keine defekte Zelle, keinen Sensor und kein
BMS. Beschädigte, heiße, aufgeblähte oder stark unausgeglichene Akkus dürfen
nicht zurückgesetzt und anschließend unbeaufsichtigt geladen werden.

Die Software wird ohne Gewährleistung bereitgestellt.
