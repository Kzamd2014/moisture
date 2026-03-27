# Project Specification: moisture

## Overview

A benchtop learning project connecting an ESP32 + capacitive soil moisture sensor to a React Native mobile app. Goal: learn the full hardware-to-mobile communication pipeline hands-on.

Solo developer project (Kelly). No cloud, no production deployment.

---

## Functional Requirements

### Phase MVP — Sensor reads over Wi-Fi
- Capacitive sensor wired to ESP32 ADC1 pin (GPIO32–39)
- Raw ADC mapped to 0–100% moisture percentage
- ESP32 HTTP server responds to `GET /moisture` with `{"moisture": 42}`
- **Done when:** browser GET returns a valid JSON moisture reading

### Phase V1 — Mobile app displays live reading
- Expo app polls `http://esp32.local/moisture` every 10 seconds
- Moisture % displayed clearly, updates live on a physical device
- **Done when:** app on a real device updates without a restart

### Phase V2 — BLE transport layer
- Eject Expo project to bare workflow
- ESP32 advertises a BLE characteristic with the moisture value
- App connects via `react-native-ble-plx` and subscribes to BLE notifications
- Wi-Fi transport remains functional alongside BLE
- **Done when:** app receives BLE notify updates without polling; Wi-Fi still functional

---

## Technical Constraints

| Constraint | Detail |
|---|---|
| ADC pin restriction | ADC2 unavailable when Wi-Fi active — sensor must use ADC1 (GPIO32–39) |
| ADC calibration | RAW_DRY ≈ 2900, RAW_WET ≈ 1200 — calibrate after hardware delivery |
| Polling interval | 10 seconds (balances responsiveness with ESP32 stability) |
| IP strategy | mDNS (`esp32.local`); fall back to hardcoded IP if mDNS fails on Android |
| BLE library | `react-native-ble-plx` (requires bare workflow — eject at Phase 2) |

---

## Out of Scope

- Push notifications or alerts
- Historical data, logging, or charts
- Multi-sensor or multi-plant support
- Cloud backend or external hosting
- App store distribution
- Any users other than the developer
