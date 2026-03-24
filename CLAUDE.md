# moisture

A benchtop learning project connecting an ESP32 + capacitive soil moisture sensor to a React Native mobile app. Goal: learn the full hardware-to-mobile communication pipeline hands-on.

**Solo developer project (Kelly). No other users, no cloud, no production deployment.**

---

## Stack

| Layer | Tech |
|---|---|
| Microcontroller | ESP32 |
| Sensor | Generic Capacitive Soil Moisture Sensor v1.2 (analog ADC, wire to GPIO32–39) |
| Firmware | Arduino (C++) |
| Transport (Phase 1) | Wi-Fi / HTTP REST |
| Transport (Phase 2) | BLE notify |
| Mobile app | React Native (Expo managed workflow → bare workflow at Phase 2) |
| BLE library | `react-native-ble-plx` (Phase 2 only) |

---

## Architecture

Data flows one direction: **Sensor → ESP32 → endpoint → app**

- ESP32 reads ADC, maps raw value to 0–100% moisture scale
- Exposes a single `GET /moisture` endpoint returning JSON
- Expo app polls that endpoint every 10 seconds and renders the value
- No broker, no cloud, no persistence
- Phase 2 adds a BLE characteristic alongside HTTP (both coexist, no removal)

---

## Project Milestones

### MVP — Sensor reads over Wi-Fi
- [ ] Capacitive sensor wired to ESP32 ADC1 pin (GPIO32–39), returning stable ADC values
- [ ] Raw ADC mapped to 0–100% moisture percentage (RAW_DRY ≈ 2900, RAW_WET ≈ 1200 — calibrate after delivery)
- [ ] ESP32 HTTP server responds to `GET /moisture` with JSON
- **Done when:** Browser `GET` returns a valid JSON moisture reading

### V1 — Mobile app displays live reading
- [ ] Expo project scaffolded, running on a physical device
- [ ] App polls `http://esp32.local/moisture` every 10 seconds, renders the value
- [ ] Basic UI — moisture % displayed clearly, updates live
- **Done when:** App on a real device updates without a restart

### V2 — BLE transport layer
- [ ] Eject Expo project to bare workflow
- [ ] ESP32 advertises a BLE characteristic with the moisture value
- [ ] React Native app connects via `react-native-ble-plx` and subscribes to BLE notifications
- [ ] Dev notes comparing Wi-Fi vs BLE approaches
- **Done when:** App receives BLE notify updates without polling; Wi-Fi transport still functional

---

## Out of Scope

- Push notifications or alerts
- Historical data, logging, or charts
- Multi-sensor or multi-plant support
- Cloud backend or external hosting
- App store distribution
- Any users other than the developer

---

## Technical Decisions

| Decision | Choice | Notes |
|---|---|---|
| Sensor | Generic Capacitive v1.2 | Wire to ADC1 (GPIO32–39); ADC2 unavailable when Wi-Fi active |
| ADC range | RAW_DRY ≈ 2900, RAW_WET ≈ 1200 | Calibrate to actual unit after delivery |
| Polling interval | 10 seconds | Balances responsiveness with ESP32 stability |
| IP strategy | mDNS (`esp32.local`) | Fall back to hardcoded IP if mDNS fails on Android |
| BLE approach | Eject to bare workflow at Phase 2 | Use `react-native-ble-plx` |

---

## Conventions

- Keep firmware and mobile app in separate top-level directories (`/firmware`, `/app`)
- Commit firmware and app changes separately
- Document ADC-to-percentage mapping constants with comments in firmware
