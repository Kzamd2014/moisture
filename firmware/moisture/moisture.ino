#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// ---------------------------------------------------------------------------
// Credentials — update before flashing
// ---------------------------------------------------------------------------
const char* WIFI_SSID     = "your-network-name";
const char* WIFI_PASSWORD = "your-password";

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

WebServer server(80);

// ---------------------------------------------------------------------------
// Read SENSOR_PIN SAMPLE_COUNT times and return the integer average.
// ---------------------------------------------------------------------------
int readAveragedADC() {
  long sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += analogRead(SENSOR_PIN);
  }
  int avg = (int)(sum / SAMPLE_COUNT);
  // Print raw average to serial for calibration (see docs/hardware_setup.md).
  Serial.printf("raw ADC (avg of %d): %d\n", SAMPLE_COUNT, avg);
  return avg;
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

  // Connect to Wi-Fi
  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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
  Serial.println("HTTP server started on port 80");
}

void loop() {
  server.handleClient();
}
