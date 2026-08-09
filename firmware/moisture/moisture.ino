#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// ---------------------------------------------------------------------------
// Credentials live in secrets.h, which is git-ignored (this repo is public).
// First-time setup:  cp secrets.h.example secrets.h  then edit it.
// Defines WIFI_SSID and WIFI_PASSWORD. Opens as its own tab in the Arduino IDE.
// ---------------------------------------------------------------------------
#include "secrets.h"

// ---------------------------------------------------------------------------
// Hardware pin (must be ADC1: GPIO32–39; ADC2 unavailable when Wi-Fi active)
// ---------------------------------------------------------------------------
const int SENSOR_PIN = 32;

// ---------------------------------------------------------------------------
// Calibration constants — update after running docs/hardware_setup.md procedure
//   RAW_DRY : ADC reading in dry air  (high value = dry)
//   RAW_WET : ADC reading submerged in water (low value = wet)
// ---------------------------------------------------------------------------
const int RAW_DRY     = 2900;
const int RAW_WET     = 1200;
const int SAMPLE_COUNT = 10;   // samples averaged per reading to reduce noise

// Serial calibration heartbeat — prints a reading this often (ms) so calibration
// is "watch the numbers" rather than "curl repeatedly". See docs/hardware_setup.md.
const unsigned long HEARTBEAT_MS = 1000;

// How long to wait for Wi-Fi before giving up and continuing anyway. Sensor
// readings still print to serial without Wi-Fi, so wiring can be verified
// independently of network problems.
const unsigned long WIFI_TIMEOUT_MS = 15000;

WebServer server(80);
bool serverStarted = false;
unsigned long lastHeartbeat = 0;

// ---------------------------------------------------------------------------
// Read SENSOR_PIN SAMPLE_COUNT times and return the integer average.
// ---------------------------------------------------------------------------
int readAveragedADC() {
  long sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += analogRead(SENSOR_PIN);
  }
  return (int)(sum / SAMPLE_COUNT);
}

// ---------------------------------------------------------------------------
// Map raw ADC value to 0–100% moisture percentage.
// Clamped: values outside the calibrated range return 0 or 100, not an error.
// ---------------------------------------------------------------------------
int toMoisturePercent(int raw) {
  int percent = map(raw, RAW_DRY, RAW_WET, 0, 100);
  return constrain(percent, 0, 100);
}

// ---------------------------------------------------------------------------
// GET /moisture handler
// Returns {"moisture_percent": N} or {"moisture_percent": 0, "error": "..."}
// Always HTTP 200 — fault conditions are signalled via the error field.
// ---------------------------------------------------------------------------
void handleMoisture() {
  int raw = readAveragedADC();

  // Fault detection: 0 means sensor not connected; >=4090 means ADC ceiling hit
  if (raw == 0 || raw >= 4090) {
    char body[80];
    snprintf(body, sizeof(body),
             "{\"moisture_percent\":0,\"error\":\"sensor fault: raw value out of range (%d)\"}",
             raw);
    server.send(200, "application/json", body);
    return;
  }

  int percent = toMoisturePercent(raw);
  char body[40];
  snprintf(body, sizeof(body), "{\"moisture_percent\":%d}", percent);
  server.send(200, "application/json", body);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi. Bounded wait — on timeout we carry on without networking so
  // the serial calibration heartbeat still works (lets you debug wiring alone).
  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWi-Fi failed (check SSID/password, and that it's 2.4 GHz).");
    Serial.println("Continuing without networking — sensor readings below still valid.");
    return;
  }

  Serial.printf("\nConnected. IP: %s\n", WiFi.localIP().toString().c_str());

  // Start mDNS — device reachable at http://esp32.local
  if (MDNS.begin("esp32")) {
    Serial.println("mDNS started: http://esp32.local");
  } else {
    Serial.println("mDNS failed — use IP address directly");
  }

  // Register route and start HTTP server
  server.on("/moisture", HTTP_GET, handleMoisture);
  server.begin();
  serverStarted = true;
  Serial.println("HTTP server started on port 80");
}

void loop() {
  if (serverStarted) {
    server.handleClient();
  }

  // Calibration heartbeat: print raw + mapped value once per HEARTBEAT_MS.
  if (millis() - lastHeartbeat >= HEARTBEAT_MS) {
    lastHeartbeat = millis();
    int raw = readAveragedADC();
    Serial.printf("raw ADC (avg of %d): %d  ->  %d%%\n",
                  SAMPLE_COUNT, raw, toMoisturePercent(raw));
  }
}
