#!/usr/bin/env python3
"""Local browser test server for the OBI-C3 embedded web interface."""

from __future__ import annotations

import json
import re
import sys
import time
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WEB_HEADER_CANDIDATES = (
    PROJECT_ROOT / "src" / "web_ui.h",
    PROJECT_ROOT / "overlay" / "ArduinoOBI" / "src" / "web_ui.h",
)
HOST = "127.0.0.1"
PORT = 8080

STATE = {
    "locked": True,
    "refresh_count": 0,
}


def load_embedded_html() -> str:
    web_header = next(
        (candidate for candidate in WEB_HEADER_CANDIDATES if candidate.exists()),
        WEB_HEADER_CANDIDATES[0],
    )
    source = web_header.read_text(encoding="utf-8")
    match = re.search(
        r'R"HTML\(\s*(.*?)\s*\)HTML";',
        source,
        flags=re.DOTALL,
    )
    if not match:
        raise RuntimeError(f"Embedded HTML was not found in {web_header}")
    return match.group(1)


def battery_data() -> dict:
    drift = (STATE["refresh_count"] % 5) * 0.001
    cells = [
        3.942 + drift,
        3.936 + drift,
        3.948 + drift,
        3.931 + drift,
        3.940 + drift,
    ]
    locked = STATE["locked"]
    return {
        "valid": True,
        "model": "BL1850B-SIM",
        "state": "LOCKED" if locked else "UNLOCKED",
        "status_code": "04" if locked else "00",
        "rom_id": "26 07 24 C3 00 00 00 01",
        "manufacturing_date": "24.07.2026",
        "battery_message": (
            "33 0F 00 F1 26 BD 13 14 58 00 00 94 94 40 21 D0 "
            "80 02 4E 23 D0 8E 45 60 1A "
            + ("04" if locked else "00")
            + " 03 02 02 0E 20 00"
        ),
        "charge_count": 127,
        "battery_type": 18,
        "capacity_ah": 5.0,
        "pack_voltage": round(sum(cells), 3),
        "cell_voltages": [round(value, 3) for value in cells],
        "cell_voltage_difference": round(max(cells) - min(cells), 3),
        "temperature_cell": round(24.6 + drift * 10, 1),
        "temperature_mosfet": round(26.2 + drift * 10, 1),
        "diagnostics_only": False,
        "read_at_ms": int(time.monotonic() * 1000),
    }


def status_data() -> dict:
    return {
        "firmware_version": "0.3.0-simulation",
        "wifi_mode": "station",
        "ssid": "OBI-Mock",
        "ip": f"{HOST}:{PORT}",
        "rssi": -42,
        "hostname": f"http://{HOST}:{PORT}",
        "uptime_seconds": int(time.monotonic()),
    }


class OBIRequestHandler(BaseHTTPRequestHandler):
    server_version = "OBI-C3-Mock/1.0"

    def log_message(self, message: str, *args) -> None:
        sys.stdout.write(
            f"{self.log_date_time_string()} {self.address_string()} "
            f"{message % args}\n"
        )

    def send_json(self, payload: dict, status: int = HTTPStatus.OK) -> None:
        body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def read_form(self) -> dict[str, str]:
        content_length = int(self.headers.get("Content-Length", "0"))
        raw_body = self.rfile.read(content_length).decode("utf-8")
        return {
            key: values[0]
            for key, values in parse_qs(raw_body, keep_blank_values=True).items()
        }

    def do_GET(self) -> None:
        if self.path == "/":
            body = load_embedded_html().encode("utf-8")
            self.send_response(HTTPStatus.OK)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(body)
            return

        if self.path == "/api/status":
            self.send_json(
                {
                    "status": status_data(),
                    "battery": {"valid": False},
                }
            )
            return

        self.send_json({"error": "Nicht gefunden"}, HTTPStatus.NOT_FOUND)

    def do_POST(self) -> None:
        if self.path in {"/api/read", "/api/read/live"}:
            STATE["refresh_count"] += 1
            self.send_json({"battery": battery_data()})
            return

        if self.path == "/api/errors/reset":
            form = self.read_form()
            if form.get("confirmation") != "RESET":
                self.send_json(
                    {"error": "Zum Bestätigen muss RESET eingegeben werden."},
                    HTTPStatus.BAD_REQUEST,
                )
                return

            before = battery_data()
            STATE["locked"] = False
            after = battery_data()
            self.send_json(
                {
                    "message": "Simulierter Fehler-Reset wurde ausgeführt.",
                    "before": {
                        "state": before["state"],
                        "status_code": before["status_code"],
                    },
                    "after": {
                        "state": after["state"],
                        "status_code": after["status_code"],
                    },
                    "battery": after,
                }
            )
            return

        if self.path == "/api/wifi":
            form = self.read_form()
            if not form.get("ssid"):
                self.send_json(
                    {"error": "WLAN-Name fehlt."},
                    HTTPStatus.BAD_REQUEST,
                )
                return
            self.send_json(
                {
                    "message": (
                        f"WLAN „{form['ssid']}“ wurde im Testmodus simuliert."
                    )
                }
            )
            return

        self.send_json({"error": "Nicht gefunden"}, HTTPStatus.NOT_FOUND)


def main() -> None:
    server = ThreadingHTTPServer((HOST, PORT), OBIRequestHandler)
    print(f"OBI-C3 Browser-Simulation: http://{HOST}:{PORT}")
    print("Beenden mit Strg+C")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nSimulation beendet.")
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
