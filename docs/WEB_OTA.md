# Firmware-Update über die OBI-C3-Webseite

OBI-C3 unterstützt Firmware-Updates direkt über den Browser. Die Update-Seite ist erreichbar unter:

```text
http://obi.local/update
```

Falls mDNS nicht funktioniert, kann stattdessen die IP-Adresse verwendet werden:

```text
http://<IP-DES-OBI-C3>/update
```

## Erstmalige Installation

Die derzeit auf dem ESP32 installierte Firmware kennt die neue OTA-Funktion noch nicht. Deshalb muss die erste OTA-fähige Version einmal per USB-C installiert werden:

```bash
pio run -e esp32-c3-web -t upload
```

Danach können weitere Updates vollständig über WLAN erfolgen.

## Neue Firmware erzeugen

Nach Änderungen am Quellcode das Projekt kompilieren:

```bash
pio run -e esp32-c3-web
```

Die hochzuladende Datei wird hier erzeugt:

```text
.pio/build/esp32-c3-web/firmware.bin
```

Nur diese `firmware.bin` über die Update-Seite hochladen. Dateien wie `bootloader.bin` oder `partitions.bin` sind nicht für das normale Web-OTA-Update vorgesehen.

## Update durchführen

1. Akku aus der OBI-C3-Aufnahme entfernen.
2. ESP32-C3 stabil über USB-C mit Strom versorgen.
3. `http://obi.local/update` öffnen.
4. `.pio/build/esp32-c3-web/firmware.bin` auswählen.
5. Zur Bestätigung `UPDATE` eingeben.
6. `Firmware installieren` auswählen.
7. Warten, bis 100 % erreicht sind und OBI-C3 automatisch neu startet.
8. Erst danach die Diagnose-Webseite erneut öffnen und den Akku einsetzen.

## Sicherheitsverhalten

- Während des Updates wird die Enable-Leitung des Akkus deaktiviert.
- Es werden nur Dateien mit der Endung `.bin` akzeptiert.
- Das Update wird in eine OTA-App-Partition geschrieben.
- Die Firmware wird erst nach vollständigem Upload aktiviert.
- Bei einem fehlgeschlagenen Upload wird kein automatischer Neustart ausgelöst.

## Hinweise

- Rechner oder Smartphone und OBI-C3 müssen sich im selben erreichbaren Netzwerk befinden.
- Während des Updates darf die USB-Stromversorgung nicht getrennt werden.
- Die Browserseite darf erst nach Abschluss beziehungsweise Neustart geschlossen werden.
- Das Update ersetzt nur die Anwendungsfirmware. Gespeicherte WLAN-Zugangsdaten in den Preferences bleiben erhalten.
