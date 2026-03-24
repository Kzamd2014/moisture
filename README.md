# moisture

A benchtop learning project connecting an ESP32 + capacitive soil moisture sensor to a React Native mobile app. The goal is to learn the full hardware-to-mobile communication pipeline hands-on.

Solo developer project. No cloud, no production deployment.

---

## How it works

1. ESP32 reads analog output from a capacitive soil moisture sensor
2. Raw ADC value is mapped to a 0–100% moisture percentage
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
/firmware   ESP32 Arduino sketch
/app        React Native (Expo) mobile app
/docs       Brainstorms and planning notes
```

---

## Milestones

### MVP — Sensor reads over Wi-Fi
- [ ] Sensor wired to ESP32 ADC1 pin (GPIO32–39)
- [ ] Raw ADC mapped to 0–100% moisture
- [ ] ESP32 HTTP server responds to `GET /moisture` with JSON

### V1 — Mobile app displays live reading
- [ ] Expo app polls `http://esp32.local/moisture` every 10 seconds
- [ ] Moisture % displayed and updates live on a physical device

### V2 — BLE transport layer
- [ ] Eject Expo to bare workflow
- [ ] ESP32 advertises BLE characteristic with moisture value
- [ ] App subscribes to BLE notifications via `react-native-ble-plx`
- [ ] Wi-Fi transport remains functional alongside BLE

---

## Hardware

**Wiring:** Connect sensor signal wire to an ADC1 pin (GPIO32–39). ADC2 pins are unavailable when Wi-Fi is active.

**ADC calibration constants** (update after measuring your specific unit):

```cpp
const int RAW_DRY = 2900;  // sensor in air
const int RAW_WET  = 1200;  // sensor submerged in water

int moisture = map(rawADC, RAW_DRY, RAW_WET, 0, 100);
moisture = constrain(moisture, 0, 100);
```

---

## Networking

The ESP32 advertises itself as `esp32.local` via mDNS. The app polls:

```
GET http://esp32.local/moisture
```

If mDNS fails on Android, fall back to the hardcoded IP address temporarily.

**Endpoint response:**

```json
{ "moisture": 42 }
```
