# moisture

A benchtop learning project connecting an ESP32 + capacitive soil moisture sensor to a React Native mobile app. The goal is to learn the full hardware-to-mobile communication pipeline hands-on.

Solo developer project. No cloud, no production deployment.

---

## How it works

1. ESP32 reads analog output from a capacitive soil moisture sensor (ADC1, GPIO32–39)
2. Raw ADC value is averaged across 10 samples and mapped to a 0–100% moisture percentage
3. ESP32 serves a `GET /moisture` JSON endpoint over Wi-Fi
4. Expo app polls the endpoint every 10 seconds and displays the live reading

---

## Stack

| Layer | Tech |
|---|---|
| Microcontroller | ESP32 |
| Sensor | Generic Capacitive Soil Moisture Sensor v1.2 |
| Firmware | Arduino (C++) |
| Transport (Phase 1) | Wi-Fi / HTTP REST |
| Transport (Phase 2) | BLE notify |
| Mobile app | React Native (Expo) |
| BLE library | `react-native-ble-plx` (Phase 2) |

---

## Project structure

```
firmware/   ESP32 Arduino sketch
app/        React Native (Expo) mobile app — Phase V1
docs/       Hardware setup, calibration, changelog
specs/      Speckit feature specs, plans, and tasks
```

---

## Milestones

### MVP — Sensor reads over Wi-Fi
- [x] Firmware written (`firmware/moisture/moisture.ino`)
- [ ] Sensor wired to ESP32 ADC1 pin (GPIO32–39) — awaiting hardware
- [ ] ADC calibration constants tuned to physical unit
- [ ] Browser `GET http://esp32.local/moisture` returns valid JSON

### V1 — Mobile app displays live reading
- [ ] Expo app polls `http://esp32.local/moisture` every 10 seconds
- [ ] Moisture % displayed and updates live on a physical device

### V2 — BLE transport layer
- [ ] Eject Expo to bare workflow
- [ ] ESP32 advertises BLE characteristic with moisture value
- [ ] App subscribes to BLE notifications via `react-native-ble-plx`
- [ ] Wi-Fi transport remains functional alongside BLE

---

## Quickstart

**First, on any fresh clone — install the credential guard:**

```
git config core.hooksPath .githooks
```

This repo is **public**, and Wi-Fi credentials are edited directly into
`firmware/moisture/moisture.ino`. The hooks in `.githooks/` are the only thing
preventing them from being committed, and `core.hooksPath` lives in `.git/config`,
which is **not cloned**. Without this command there is no guard.

Optionally, so the guard can catch leaks through any file (pasted serial logs,
sketch copies), record your real values in a local untracked denylist:

```
printf '%s\n' 'your-actual-ssid' 'your-actual-password' > .git/secrets-denylist
chmod 600 .git/secrets-denylist
```

See [`firmware/README.md`](firmware/README.md) for wiring, flash instructions, and troubleshooting.

See [`docs/hardware_setup.md`](docs/hardware_setup.md) for the ADC calibration procedure.

---

## Endpoint

```
GET http://esp32.local/moisture
```

Fallback (Android / mDNS unavailable):

```
GET http://<device-ip>/moisture
```

Response:

```json
{"moisture_percent": 42}
```

Fault response:

```json
{"moisture_percent": 0, "error": "sensor fault: raw value out of range (4095)"}
```

---

## Calibration constants

Update these in `firmware/moisture/moisture.ino` after running the calibration procedure:

```cpp
const int RAW_DRY    = 2900;  // ADC reading in dry air
const int RAW_WET    = 1200;  // ADC reading submerged in water
const int SAMPLE_COUNT = 10;  // samples averaged per reading
```
