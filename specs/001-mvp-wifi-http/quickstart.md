# Quickstart: MVP Wi-Fi HTTP Moisture Endpoint

**Feature**: `001-mvp-wifi-http`
**Date**: 2026-05-17

This guide covers flashing the firmware and verifying the endpoint works from a browser.

## Prerequisites

- ESP32 development board with capacitive soil moisture sensor wired to an ADC1 pin (GPIO32–39)
- Arduino IDE (2.x) or PlatformIO with ESP32 Arduino Core installed
- Wi-Fi network credentials
- A browser on the same local network as the ESP32

## Step 1 — Wire the Sensor

Connect the sensor's analog output (AOUT) to any ADC1 pin (GPIO32–39).
See `docs/hardware_setup.md` for the full wiring diagram.

## Step 2 — Configure Firmware Constants

Open `firmware/moisture/moisture.ino`. Update the following at the top of the file:

```cpp
const char* WIFI_SSID     = "your-network-name";
const char* WIFI_PASSWORD = "your-password";

// ADC pin (must be GPIO32–39)
const int SENSOR_PIN = 32;

// Calibration — update after running the calibration procedure in docs/hardware_setup.md
const int RAW_DRY = 2900;   // ADC value in dry air
const int RAW_WET = 1200;   // ADC value submerged in water
```

## Step 3 — Flash the Firmware

1. Select the correct ESP32 board in Arduino IDE: **Tools → Board → ESP32 Dev Module**
2. Select the correct serial port: **Tools → Port → /dev/cu.usbserial-xxx** (macOS) or COM# (Windows)
3. Click **Upload**

The serial monitor (115200 baud) will print the assigned IP address after Wi-Fi connects.
Note this IP — you will need it as a fallback in Step 5.

## Step 4 — Verify via mDNS

Open a browser and navigate to:

```
http://esp32.local/moisture
```

Expected response:

```json
{"moisture_percent": 42}
```

The exact value depends on current sensor conditions.

## Step 5 — Fallback: Use IP Address Directly

If `esp32.local` does not resolve (common on Android, some Windows setups):

1. Open the Arduino serial monitor at 115200 baud
2. Note the IP printed on startup, e.g. `192.168.1.105`
3. Navigate to `http://192.168.1.105/moisture`

## Step 6 — Validate the Reading Range

To confirm calibration is working:

| Condition | Expected `moisture_percent` |
|-----------|----------------------------|
| Sensor in dry air | 0–10 |
| Sensor in dry soil | ~20–40 |
| Sensor in moist soil | ~50–70 |
| Sensor submerged in water | 90–100 |

If readings are inverted (wet = 0, dry = 100), swap `RAW_DRY` and `RAW_WET` and reflash.

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| `esp32.local` doesn't resolve | mDNS not supported on client OS | Use IP address directly (Step 5) |
| `moisture_percent` always 0 or 100 | Calibration constants wrong | Re-run calibration procedure |
| `"error": "sensor fault..."` in response | Sensor disconnected or wrong pin | Check wiring, verify SENSOR_PIN |
| No response at all | Device not connected to Wi-Fi | Check SSID/password, check serial monitor |
| Reading is noisy / jumpy | Not averaging enough samples | Increase `SAMPLE_COUNT` in firmware |
