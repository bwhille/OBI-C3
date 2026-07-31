#include <Arduino.h>
#include "OneWire2.h"

/** OBI interface firmware version. */
#define ARDUINO_OBI_VERSION_MAJOR 0
#define ARDUINO_OBI_VERSION_MINOR 4
#define ARDUINO_OBI_VERSION_PATCH 3

#ifdef ESP_BUILD
#define ONEWIRE_PIN ESP_OW_PIN
#define ENABLE_PIN ESP_EN_PIN
#else
#define ONEWIRE_PIN 6
#define ENABLE_PIN 8
#endif

OneWire makita(ONEWIRE_PIN);

void cmd_and_read_33(byte *cmd, uint8_t cmd_len, byte *rsp, uint8_t rsp_len) {
    makita.reset();
    delayMicroseconds(400);
    makita.write(0x33, 0);

    for (uint8_t i = 0; i < 8; i++) {
        delayMicroseconds(90);
        rsp[i] = makita.read();
    }

    for (uint8_t i = 0; i < cmd_len; i++) {
        delayMicroseconds(90);
        makita.write(cmd[i], 0);
    }

    for (uint16_t i = 8; i < static_cast<uint16_t>(rsp_len) + 8; i++) {
        delayMicroseconds(90);
        rsp[i] = makita.read();
    }
}

void cmd_and_read_cc(byte *cmd, uint8_t cmd_len, byte *rsp, uint8_t rsp_len) {
    makita.reset();
    delayMicroseconds(400);
    makita.write(0xCC, 0);

    for (uint8_t i = 0; i < cmd_len; i++) {
        delayMicroseconds(90);
        makita.write(cmd[i], 0);
    }

    for (uint8_t i = 0; i < rsp_len; i++) {
        delayMicroseconds(90);
        rsp[i] = makita.read();
    }
}

void cmd_and_read(byte *cmd, uint8_t cmd_len, byte *rsp, uint8_t rsp_len) {
    makita.reset();
    delayMicroseconds(400);

    for (uint8_t i = 0; i < cmd_len; i++) {
        delayMicroseconds(90);
        makita.write(cmd[i], 0);
    }

    for (uint8_t i = 0; i < rsp_len; i++) {
        delayMicroseconds(90);
        rsp[i] = makita.read();
    }
}

bool execute_obi_packet(
    const byte *packet,
    size_t packet_len,
    byte *response,
    size_t response_capacity,
    String &error
) {
    if (packet_len < 4 || packet[0] != 0x01) {
        error = "Ungueltiges OBI-Kommando.";
        return false;
    }

    const uint8_t data_len = packet[1];
    const uint8_t rsp_len = packet[2];
    const uint8_t cmd = packet[3];

    if (packet_len < static_cast<size_t>(4 + data_len)) {
        error = "OBI-Kommando ist unvollstaendig.";
        return false;
    }
    if (response_capacity < static_cast<size_t>(rsp_len + 2)) {
        error = "Antwortpuffer ist zu klein.";
        return false;
    }

    byte data[255] = {0};
    byte payload[255] = {0};
    for (uint8_t i = 0; i < data_len; i++) {
        data[i] = packet[4 + i];
    }

    digitalWrite(ENABLE_PIN, HIGH);
    delay(400);

    switch (cmd) {
        case 0x01:
            payload[0] = ARDUINO_OBI_VERSION_MAJOR;
            payload[1] = ARDUINO_OBI_VERSION_MINOR;
            payload[2] = ARDUINO_OBI_VERSION_PATCH;
            break;

        case 0x31:
            makita.reset();
            delayMicroseconds(400);
            makita.write(0xCC, 0);
            delayMicroseconds(90);
            makita.write(0x99, 0);
            delay(400);
            makita.reset();
            delayMicroseconds(400);
            makita.write(0x31, 0);
            delayMicroseconds(90);
            payload[1] = makita.read();
            delayMicroseconds(90);
            payload[0] = makita.read();
            delayMicroseconds(90);
            break;

        case 0x32:
            makita.reset();
            delayMicroseconds(400);
            makita.write(0xCC, 0);
            delayMicroseconds(90);
            makita.write(0x99, 0);
            delay(400);
            makita.reset();
            delayMicroseconds(400);
            makita.write(0x32, 0);
            delayMicroseconds(90);
            payload[1] = makita.read();
            delayMicroseconds(90);
            payload[0] = makita.read();
            delayMicroseconds(90);
            break;

        case 0x33:
            cmd_and_read_33(data, data_len, payload, rsp_len);
            break;

        case 0xCC:
            cmd_and_read_cc(data, data_len, payload, rsp_len);
            break;

        default:
            digitalWrite(ENABLE_PIN, LOW);
            error = "Nicht unterstuetztes OBI-Kommando.";
            return false;
    }

    response[0] = cmd;
    response[1] = rsp_len;
    for (uint8_t i = 0; i < rsp_len; i++) {
        response[i + 2] = payload[i];
    }

    digitalWrite(ENABLE_PIN, LOW);
    return true;
}

bool response_is_valid(const byte *response, size_t length) {
    if (length <= 2) {
        return true;
    }

    bool all_ff = true;
    for (size_t i = 2; i < length; i++) {
        if (response[i] != 0xFF) {
            all_ff = false;
            break;
        }
    }
    return !all_ff;
}

bool request_obi(
    const byte *packet,
    size_t packet_len,
    byte *response,
    size_t response_capacity,
    String &error,
    uint8_t attempts = 2
) {
    const size_t expected_length = packet[2] + 2;
    for (uint8_t attempt = 0; attempt < attempts; attempt++) {
        if (!execute_obi_packet(
                packet,
                packet_len,
                response,
                response_capacity,
                error
            )) {
            return false;
        }

        if (response_is_valid(response, expected_length)) {
            return true;
        }

        error = "Keine gueltige Akku-Antwort. Sitz und Verdrahtung pruefen.";
        delay(100);
    }
    return false;
}

#ifndef OBI_WEB_BUILD

void send_usb(byte *response, byte response_len) {
    for (uint8_t i = 0; i < response_len; i++) {
        Serial.write(response[i]);
    }
}

void read_usb() {
    if (Serial.available() < 4) {
        return;
    }

    const byte start = Serial.read();
    if (start != 0x01) {
        return;
    }

    const byte len = Serial.read();
    const byte rsp_len = Serial.read();
    const byte cmd = Serial.read();
    byte packet[259] = {0x01, len, rsp_len, cmd};

    for (uint8_t i = 0; i < len; i++) {
        while (Serial.available() < 1) {
            delay(1);
        }
        packet[4 + i] = Serial.read();
    }

    byte response[257] = {0};
    String error;
    if (execute_obi_packet(
            packet,
            static_cast<size_t>(4 + len),
            response,
            sizeof(response),
            error
        )) {
        send_usb(response, rsp_len + 2);
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW);
}

void loop() {
    read_usb();
}

#else

#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <Update.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "web_ui.h"

WebServer server(80);
Preferences preferences;

static const char *UPDATE_API_URL =
    "https://api.github.com/repos/bwhille/OBI-C3/releases/latest";
static const char *UPDATE_ASSET_NAME = "firmware.bin";

static const byte MODEL_CMD[] = {0x01, 0x02, 0x10, 0xCC, 0xDC, 0x0C};
static const byte READ_DATA_REQUEST[] = {
    0x01, 0x04, 0x1D, 0xCC, 0xD7, 0x00, 0x00, 0xFF
};
static const byte TESTMODE_CMD[] = {
    0x01, 0x03, 0x09, 0x33, 0xD9, 0x96, 0xA5
};
static const byte RESET_ERROR_CMD[] = {
    0x01, 0x02, 0x09, 0x33, 0xDA, 0x04
};
static const byte READ_MSG_CMD[] = {
    0x01, 0x02, 0x28, 0x33, 0xAA, 0x00
};
static const byte CLEAR_CMD[] = {
    0x01, 0x02, 0x00, 0xCC, 0xF0, 0x00
};
static const byte F0513_MODEL_CMD[] = {
    0x01, 0x00, 0x02, 0x31
};
static const byte F0513_TEMP_CMD[] = {
    0x01, 0x01, 0x02, 0xCC, 0x52
};
static const byte F0513_VCELL_CMDS[5][5] = {
    {0x01, 0x01, 0x02, 0xCC, 0x31},
    {0x01, 0x01, 0x02, 0xCC, 0x32},
    {0x01, 0x01, 0x02, 0xCC, 0x33},
    {0x01, 0x01, 0x02, 0xCC, 0x34},
    {0x01, 0x01, 0x02, 0xCC, 0x35}
};

struct BatteryData {
    bool valid = false;
    bool diagnostics_only = false;
    String model;
    String state;
    String status_code;
    String rom_id;
    String manufacturing_date;
    String battery_message;
    uint16_t charge_count = 0;
    uint8_t battery_type = 0;
    float capacity_ah = 0;
    float pack_voltage = 0;
    float cell_voltages[5] = {0, 0, 0, 0, 0};
    float cell_voltage_difference = 0;
    float temperature_cell = 0;
    float temperature_mosfet = 0;
    bool has_mosfet_temperature = false;
    unsigned long read_at_ms = 0;
};

BatteryData battery;
bool access_point_mode = false;
String access_point_name;
#ifdef OBI_MOCK_BATTERY
bool mock_battery_locked = true;
uint32_t mock_refresh_count = 0;
#endif

uint8_t nibble_swap(uint8_t value) {
    return ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
}

uint16_t little_endian_u16(const byte *data) {
    return static_cast<uint16_t>(data[0]) |
           (static_cast<uint16_t>(data[1]) << 8);
}

String hex_range(const byte *data, size_t start, size_t end) {
    String value;
    for (size_t i = start; i < end; i++) {
        if (i > start) {
            value += ' ';
        }
        if (data[i] < 0x10) {
            value += '0';
        }
        value += String(data[i], HEX);
    }
    value.toUpperCase();
    return value;
}

String two_digits(uint8_t value) {
    return value < 10 ? "0" + String(value) : String(value);
}

bool is_printable_model(const byte *response) {
    for (size_t i = 2; i < 9; i++) {
        if (response[i] == 0 || response[i] == 0xFF) {
            continue;
        }
        if (response[i] < 0x20 || response[i] > 0x7E) {
            return false;
        }
    }
    return true;
}

bool read_static_data(String &error) {
#ifdef OBI_MOCK_BATTERY
    battery.valid = true;
    battery.diagnostics_only = false;
    battery.model = "BL1850B-SIM";
    battery.state = mock_battery_locked ? "LOCKED" : "UNLOCKED";
    battery.status_code = mock_battery_locked ? "04" : "00";
    battery.rom_id = "26 07 24 C3 00 00 00 01";
    battery.manufacturing_date = "24.07.2026";
    battery.battery_message =
        mock_battery_locked
            ? "33 0F 00 F1 26 BD 13 14 58 00 00 94 94 40 21 D0 "
              "80 02 4E 23 D0 8E 45 60 1A 04 03 12 02 0E 20 00"
            : "33 0F 00 F1 26 BD 13 14 58 00 00 94 94 40 21 D0 "
              "80 02 4E 23 D0 8E 45 60 1A 00 03 02 02 0E 20 00";
    battery.charge_count = 127;
    battery.battery_type = 18;
    battery.capacity_ah = 5.0f;
    battery.read_at_ms = millis();
    error = "";
    return true;
#else
    byte response[64] = {0};
    if (!request_obi(
            READ_MSG_CMD,
            sizeof(READ_MSG_CMD),
            response,
            sizeof(response),
            error
        )) {
        return false;
    }

    battery.rom_id = hex_range(response, 2, 10);
    battery.battery_message = hex_range(response, 10, 42);

    byte swapped[2] = {
        nibble_swap(response[36]),
        nibble_swap(response[37])
    };
    battery.charge_count =
        (static_cast<uint16_t>(swapped[0]) << 8 | swapped[1]) & 0x0FFF;
    battery.state = (response[30] & 0x0F) > 0 ? "LOCKED" : "UNLOCKED";
    battery.status_code =
        response[29] < 0x10 ? "0" + String(response[29], HEX)
                            : String(response[29], HEX);
    battery.status_code.toUpperCase();
    battery.manufacturing_date =
        two_digits(response[4]) + "." +
        two_digits(response[3]) + ".20" +
        two_digits(response[2]);
    battery.capacity_ah = nibble_swap(response[26]) / 10.0f;
    battery.battery_type = nibble_swap(response[21]);

    byte model_response[32] = {0};
    String standard_error;
    bool standard_model = request_obi(
        MODEL_CMD,
        sizeof(MODEL_CMD),
        model_response,
        sizeof(model_response),
        standard_error
    );

    if (standard_model && is_printable_model(model_response)) {
        battery.model = "";
        for (size_t i = 2; i < 9; i++) {
            if (model_response[i] != 0 && model_response[i] != 0xFF) {
                battery.model += static_cast<char>(model_response[i]);
            }
        }
        battery.model.trim();
        standard_model = battery.model.length() > 0;
    }

    if (standard_model) {
        battery.diagnostics_only = false;
    } else {
        byte f0513_response[8] = {0};
        if (!request_obi(
                F0513_MODEL_CMD,
                sizeof(F0513_MODEL_CMD),
                f0513_response,
                sizeof(f0513_response),
                error
            )) {
            error = "Akku erkannt, Modell aber nicht unterstuetzt. " + error;
            return false;
        }

        byte clear_response[4] = {0};
        request_obi(
            CLEAR_CMD,
            sizeof(CLEAR_CMD),
            clear_response,
            sizeof(clear_response),
            error
        );
        battery.model =
            "BL" + String(f0513_response[2], HEX) +
            String(f0513_response[3], HEX);
        battery.model.toUpperCase();
        battery.diagnostics_only = true;
    }

    battery.valid = true;
    battery.read_at_ms = millis();
    return true;
#endif
}

bool read_live_data(String &error) {
#ifdef OBI_MOCK_BATTERY
    if (!battery.valid && !read_static_data(error)) {
        return false;
    }

    mock_refresh_count++;
    const float drift =
        static_cast<int32_t>(mock_refresh_count % 5) * 0.001f;
    battery.cell_voltages[0] = 3.942f + drift;
    battery.cell_voltages[1] = 3.936f + drift;
    battery.cell_voltages[2] = 3.948f + drift;
    battery.cell_voltages[3] = 3.931f + drift;
    battery.cell_voltages[4] = 3.940f + drift;
    battery.pack_voltage = 0;
    for (float voltage : battery.cell_voltages) {
        battery.pack_voltage += voltage;
    }
    battery.cell_voltage_difference =
        battery.cell_voltages[2] - battery.cell_voltages[3];
    battery.temperature_cell = 24.6f + drift * 10;
    battery.temperature_mosfet = 26.2f + drift * 10;
    battery.has_mosfet_temperature = true;
    battery.read_at_ms = millis();
    error = "";
    return true;
#else
    if (!battery.valid && !read_static_data(error)) {
        return false;
    }

    if (battery.diagnostics_only) {
        byte clear_response[4] = {0};
        request_obi(
            CLEAR_CMD,
            sizeof(CLEAR_CMD),
            clear_response,
            sizeof(clear_response),
            error
        );
        request_obi(
            CLEAR_CMD,
            sizeof(CLEAR_CMD),
            clear_response,
            sizeof(clear_response),
            error
        );

        battery.pack_voltage = 0;
        for (size_t i = 0; i < 5; i++) {
            byte response[8] = {0};
            if (!request_obi(
                    F0513_VCELL_CMDS[i],
                    sizeof(F0513_VCELL_CMDS[i]),
                    response,
                    sizeof(response),
                    error
                )) {
                return false;
            }
            battery.cell_voltages[i] =
                little_endian_u16(&response[2]) / 1000.0f;
            battery.pack_voltage += battery.cell_voltages[i];
        }

        byte temp_response[8] = {0};
        if (!request_obi(
                F0513_TEMP_CMD,
                sizeof(F0513_TEMP_CMD),
                temp_response,
                sizeof(temp_response),
                error
            )) {
            return false;
        }
        battery.temperature_cell =
            little_endian_u16(&temp_response[2]) / 100.0f;
        battery.has_mosfet_temperature = false;
    } else {
        byte response[40] = {0};
        if (!request_obi(
                READ_DATA_REQUEST,
                sizeof(READ_DATA_REQUEST),
                response,
                sizeof(response),
                error
            )) {
            return false;
        }

        battery.pack_voltage =
            little_endian_u16(&response[2]) / 1000.0f;
        for (size_t i = 0; i < 5; i++) {
            battery.cell_voltages[i] =
                little_endian_u16(&response[4 + i * 2]) / 1000.0f;
        }
        battery.temperature_cell =
            little_endian_u16(&response[16]) / 100.0f;
        battery.temperature_mosfet =
            little_endian_u16(&response[18]) / 100.0f;
        battery.has_mosfet_temperature = true;
    }

    float minimum = battery.cell_voltages[0];
    float maximum = battery.cell_voltages[0];
    for (size_t i = 1; i < 5; i++) {
        minimum = min(minimum, battery.cell_voltages[i]);
        maximum = max(maximum, battery.cell_voltages[i]);
    }
    battery.cell_voltage_difference = maximum - minimum;
    battery.read_at_ms = millis();
    return true;
#endif
}

void add_battery_json(JsonObject target) {
    target["valid"] = battery.valid;
    if (!battery.valid) {
        return;
    }

    target["model"] = battery.model;
    target["state"] = battery.state;
    target["status_code"] = battery.status_code;
    target["rom_id"] = battery.rom_id;
    target["manufacturing_date"] = battery.manufacturing_date;
    target["battery_message"] = battery.battery_message;
    target["charge_count"] = battery.charge_count;
    target["battery_type"] = battery.battery_type;
    target["capacity_ah"] = battery.capacity_ah;
    target["pack_voltage"] = battery.pack_voltage;
    target["cell_voltage_difference"] = battery.cell_voltage_difference;
    target["temperature_cell"] = battery.temperature_cell;
    if (battery.has_mosfet_temperature) {
        target["temperature_mosfet"] = battery.temperature_mosfet;
    } else {
        target["temperature_mosfet"] = nullptr;
    }
    target["diagnostics_only"] = battery.diagnostics_only;
    target["read_at_ms"] = battery.read_at_ms;

    JsonArray cells = target["cell_voltages"].to<JsonArray>();
    for (float voltage : battery.cell_voltages) {
        cells.add(voltage);
    }
}

void send_json(JsonDocument &document, int status = 200);
void send_error(const String &message, int status = 400);

void add_status_json(JsonObject target) {
    target["firmware_version"] =
        String(ARDUINO_OBI_VERSION_MAJOR) + "." +
        String(ARDUINO_OBI_VERSION_MINOR) + "." +
        String(ARDUINO_OBI_VERSION_PATCH);
    target["wifi_mode"] = access_point_mode ? "ap" : "station";
    target["ssid"] =
        access_point_mode ? access_point_name : WiFi.SSID();
    target["ip"] =
        access_point_mode
            ? WiFi.softAPIP().toString()
            : WiFi.localIP().toString();
    if (access_point_mode) {
        target["rssi"] = nullptr;
    } else {
        target["rssi"] = WiFi.RSSI();
    }
    target["hostname"] = "obi.local";
    target["uptime_seconds"] = millis() / 1000;
}

String firmware_version() {
    return String(ARDUINO_OBI_VERSION_MAJOR) + "." +
           String(ARDUINO_OBI_VERSION_MINOR) + "." +
           String(ARDUINO_OBI_VERSION_PATCH);
}

int compare_versions(const String &left, const String &right) {
    int left_values[3] = {0, 0, 0};
    int right_values[3] = {0, 0, 0};
    sscanf(left.c_str(), "%d.%d.%d", &left_values[0], &left_values[1], &left_values[2]);
    sscanf(right.c_str(), "%d.%d.%d", &right_values[0], &right_values[1], &right_values[2]);
    for (size_t i = 0; i < 3; i++) {
        if (left_values[i] != right_values[i]) {
            return left_values[i] > right_values[i] ? 1 : -1;
        }
    }
    return 0;
}

bool fetch_update_info(
    String &latest_version,
    String &download_url,
    String &release_url,
    String &error
) {
    if (access_point_mode || WiFi.status() != WL_CONNECTED) {
        error = "Für Updates muss das Gerät mit dem Heim-WLAN verbunden sein.";
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setRedirectLimit(3);
    if (!http.begin(client, UPDATE_API_URL)) {
        error = "GitHub-Updateprüfung konnte nicht gestartet werden.";
        return false;
    }
    http.addHeader("User-Agent", "OBI-C3");
    const int status = http.GET();
    if (status == HTTP_CODE_NOT_FOUND) {
        error = "NO_RELEASE";
        http.end();
        return false;
    }
    if (status != HTTP_CODE_OK) {
        error = "GitHub-Updateprüfung fehlgeschlagen: HTTP " + String(status);
        http.end();
        return false;
    }

    JsonDocument document;
    const DeserializationError parse_error = deserializeJson(document, http.getString());
    http.end();
    if (parse_error) {
        error = "Die GitHub-Updateinformationen sind ungültig.";
        return false;
    }

    latest_version = document["tag_name"].as<String>();
    if (latest_version.startsWith("v") || latest_version.startsWith("V")) {
        latest_version.remove(0, 1);
    }
    release_url = document["html_url"].as<String>();
    for (JsonObject asset : document["assets"].as<JsonArray>()) {
        if (asset["name"].as<String>() == UPDATE_ASSET_NAME) {
            download_url = asset["browser_download_url"].as<String>();
            break;
        }
    }
    if (latest_version.length() == 0 || download_url.length() == 0) {
        error = "Im neuesten GitHub-Release wurde keine firmware.bin gefunden.";
        return false;
    }
    return true;
}

void handle_update_check() {
    String latest_version;
    String download_url;
    String release_url;
    String error;
    if (!fetch_update_info(latest_version, download_url, release_url, error)) {
        if (error == "NO_RELEASE") {
            JsonDocument document;
            document["current_version"] = firmware_version();
            document["update_available"] = false;
            document["message"] = "Noch kein Firmware-Release veröffentlicht.";
            send_json(document);
            return;
        }
        send_error(error, 503);
        return;
    }

    JsonDocument document;
    document["current_version"] = firmware_version();
    document["latest_version"] = latest_version;
    document["update_available"] = compare_versions(latest_version, firmware_version()) > 0;
    document["release_url"] = release_url;
    send_json(document);
}

void handle_update_install() {
    String latest_version;
    String download_url;
    String release_url;
    String error;
    if (!fetch_update_info(latest_version, download_url, release_url, error)) {
        if (error == "NO_RELEASE") {
            send_error("Noch kein Firmware-Release veröffentlicht.", 404);
            return;
        }
        send_error(error, 503);
        return;
    }
    if (compare_versions(latest_version, firmware_version()) <= 0) {
        send_error("Die Firmware ist bereits aktuell.", 409);
        return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setRedirectLimit(3);
    if (!http.begin(client, download_url)) {
        send_error("Firmware-Download konnte nicht gestartet werden.", 503);
        return;
    }
    http.addHeader("User-Agent", "OBI-C3");
    const int status = http.GET();
    if (status != HTTP_CODE_OK) {
        error = "Firmware-Download fehlgeschlagen: HTTP " + String(status);
        http.end();
        send_error(error, 503);
        return;
    }

    const int content_length = http.getSize();
    if (content_length <= 0 || !Update.begin(static_cast<size_t>(content_length))) {
        http.end();
        send_error("Firmwaregröße ist ungültig oder zu groß.", 507);
        return;
    }
    const size_t written = Update.writeStream(*http.getStreamPtr());
    const bool completed = written == static_cast<size_t>(content_length) && Update.end(true);
    http.end();
    if (!completed) {
        Update.abort();
        send_error("Firmware konnte nicht vollständig installiert werden.", 500);
        return;
    }

    JsonDocument document;
    document["message"] = "Update installiert. Das Gerät startet jetzt neu.";
    document["version"] = latest_version;
    send_json(document);
    delay(700);
    ESP.restart();
}

void send_json(JsonDocument &document, int status) {
    String output;
    serializeJson(document, output);
    server.send(status, "application/json; charset=utf-8", output);
}

void send_error(const String &message, int status) {
    JsonDocument document;
    document["error"] = message;
    send_json(document, status);
}

void handle_status() {
    JsonDocument document;
    JsonObject status = document["status"].to<JsonObject>();
    add_status_json(status);
    JsonObject battery_json = document["battery"].to<JsonObject>();
    add_battery_json(battery_json);
    send_json(document);
}

void handle_read_all() {
    String error;
    if (!read_static_data(error) || !read_live_data(error)) {
        send_error(error, 502);
        return;
    }

    JsonDocument document;
    JsonObject battery_json = document["battery"].to<JsonObject>();
    add_battery_json(battery_json);
    send_json(document);
}

void handle_read_live() {
    String error;
    if (!read_live_data(error)) {
        send_error(error, 502);
        return;
    }

    JsonDocument document;
    JsonObject battery_json = document["battery"].to<JsonObject>();
    add_battery_json(battery_json);
    send_json(document);
}

void handle_reset_errors() {
    if (!server.hasArg("confirmation") ||
        server.arg("confirmation") != "RESET") {
        send_error("Zum Bestaetigen muss RESET eingegeben werden.", 400);
        return;
    }

    String error;
    if (!read_static_data(error)) {
        send_error(error, 502);
        return;
    }
    if (battery.diagnostics_only) {
        send_error(
            "F0513 wird nur diagnostisch unterstuetzt. Reset ist gesperrt.",
            422
        );
        return;
    }

    const String before_state = battery.state;
    const String before_code = battery.status_code;
#ifdef OBI_MOCK_BATTERY
    mock_battery_locked = false;
    battery.valid = false;
    if (!read_static_data(error) || !read_live_data(error)) {
        send_error("Simulierter Reset konnte nicht ausgewertet werden.", 500);
        return;
    }
#else
    byte response[16] = {0};

    if (!request_obi(
            TESTMODE_CMD,
            sizeof(TESTMODE_CMD),
            response,
            sizeof(response),
            error
        ) ||
        !request_obi(
            RESET_ERROR_CMD,
            sizeof(RESET_ERROR_CMD),
            response,
            sizeof(response),
            error
        )) {
        send_error(error, 502);
        return;
    }

    delay(250);
    battery.valid = false;
    if (!read_static_data(error)) {
        send_error(
            "Reset gesendet, anschliessendes Auslesen fehlgeschlagen: " + error,
            502
        );
        return;
    }
#endif

    JsonDocument document;
    document["message"] =
#ifdef OBI_MOCK_BATTERY
        "Simulierter Fehler-Reset wurde ausgefuehrt.";
#else
        "Fehler-Reset wurde gesendet und kontrolliert.";
#endif
    document["before"]["state"] = before_state;
    document["before"]["status_code"] = before_code;
    document["after"]["state"] = battery.state;
    document["after"]["status_code"] = battery.status_code;
    JsonObject battery_json = document["battery"].to<JsonObject>();
    add_battery_json(battery_json);
    send_json(document);
}

void handle_wifi_save() {
    if (!server.hasArg("ssid") || server.arg("ssid").length() == 0) {
        send_error("WLAN-Name fehlt.");
        return;
    }

    preferences.begin("obi-wifi", false);
    preferences.putString("ssid", server.arg("ssid"));
    preferences.putString("password", server.arg("password"));
    preferences.end();

    JsonDocument document;
    document["message"] = "WLAN gespeichert. ESP32 startet neu.";
    send_json(document);
    delay(700);
    ESP.restart();
}

void handle_wifi_clear() {
    preferences.begin("obi-wifi", false);
    preferences.clear();
    preferences.end();

    JsonDocument document;
    document["message"] = "WLAN geloescht. ESP32 startet im Setup-Modus.";
    send_json(document);
    delay(700);
    ESP.restart();
}

void start_access_point() {
    uint64_t chip_id = ESP.getEfuseMac();
    char suffix[7];
    snprintf(suffix, sizeof(suffix), "%06llX", chip_id & 0xFFFFFF);
    access_point_name = "OBI-Setup-" + String(suffix);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(access_point_name.c_str(), "obi-setup");
    access_point_mode = true;

    Serial.println("OBI Setup Access Point gestartet:");
    Serial.println("  SSID: " + access_point_name);
    Serial.println("  Passwort: obi-setup");
    Serial.println("  Adresse: http://" + WiFi.softAPIP().toString());
}

void setup_wifi() {
#ifdef OBI_WOKWI_WIFI
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("obi");
    WiFi.begin("Wokwi-GUEST", "", 6);

    Serial.print("Verbinde mit Wokwi-GUEST");
    for (uint8_t attempt = 0;
         attempt < 30 && WiFi.status() != WL_CONNECTED;
         attempt++) {
        delay(500);
        Serial.print('.');
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        access_point_mode = false;
        Serial.println("Wokwi-WLAN verbunden.");
        Serial.println("Adresse: http://" + WiFi.localIP().toString());
        return;
    }

    Serial.println("Wokwi-WLAN konnte nicht verbunden werden.");
    start_access_point();
    return;
#else
    preferences.begin("obi-wifi", true);
    const String ssid = preferences.getString("ssid", "");
    const String password = preferences.getString("password", "");
    preferences.end();

    if (ssid.length() == 0) {
        start_access_point();
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.setHostname("obi");
    WiFi.begin(ssid.c_str(), password.c_str());

    Serial.print("Verbinde mit WLAN ");
    Serial.print(ssid);
    for (uint8_t attempt = 0;
         attempt < 30 && WiFi.status() != WL_CONNECTED;
         attempt++) {
        delay(500);
        Serial.print('.');
    }
    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WLAN-Verbindung fehlgeschlagen.");
        start_access_point();
        return;
    }

    access_point_mode = false;
    if (MDNS.begin("obi")) {
        MDNS.addService("http", "tcp", 80);
    }

    Serial.println("WLAN verbunden.");
    Serial.println("Adresse: http://" + WiFi.localIP().toString());
    Serial.println("mDNS: http://obi.local");
#endif
}

void setup_web_server() {
    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
    });
    server.on("/api/status", HTTP_GET, handle_status);
    server.on("/api/read", HTTP_POST, handle_read_all);
    server.on("/api/read/live", HTTP_POST, handle_read_live);
    server.on("/api/update/check", HTTP_GET, handle_update_check);
    server.on("/api/update/install", HTTP_POST, handle_update_install);
    server.on("/api/errors/reset", HTTP_POST, handle_reset_errors);
    server.on("/api/wifi", HTTP_POST, handle_wifi_save);
    server.on("/api/wifi/clear", HTTP_POST, handle_wifi_clear);
    server.onNotFound([]() {
        server.send(404, "application/json", "{\"error\":\"Nicht gefunden\"}");
    });
    server.begin();
    Serial.println("OBI Webserver gestartet.");
}

void setup() {
    Serial.begin(115200);
    delay(300);
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW);

    Serial.println();
    Serial.println("OBI Web startet.");
    setup_wifi();
    setup_web_server();
}

void loop() {
    server.handleClient();
    delay(2);
}

#endif
