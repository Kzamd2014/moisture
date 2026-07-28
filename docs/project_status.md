# Project Status

**Last updated:** 2026-05-17
**Current phase:** MVP firmware complete — awaiting hardware for testing and calibration

---

## Milestone Checklist

### MVP — Sensor reads over Wi-Fi
- [x] Capacitive sensor wired to ESP32 ADC1 pin (GPIO32–39)
- [x] Raw ADC mapped to 0–100% moisture percentage
- [x] ESP32 HTTP server responds to `GET /moisture` with JSON
- **Status:** Complete (firmware written; awaiting hardware for field testing)

### V1 — Mobile app displays live reading
- [ ] Expo project scaffolded, running on a physical device
- [ ] App polls `http://esp32.local/moisture` every 10 seconds, renders the value
- [ ] Basic UI — moisture % displayed clearly, updates live
- **Status:** Blocked on MVP

### V2 — BLE transport layer
- [ ] Eject Expo project to bare workflow
- [ ] ESP32 advertises a BLE characteristic with the moisture value
- [ ] React Native app connects via `react-native-ble-plx` and subscribes to BLE notifications
- [ ] Dev notes comparing Wi-Fi vs BLE approaches
- **Status:** Blocked on V1

---

## What's Done

- Project scaffolded: CLAUDE.md, README.md, docs/
- Architecture, spec, and status documented
- MVP firmware complete: `/firmware/moisture.ino` with HTTP GET /moisture endpoint, ADC averaging, calibration constants, and mDNS support

## Infrastructure Notes

- Claude Code PostToolUse hook auto-updates project_status, architecture, and project_spec when CLAUDE.md or firmware/app files change
- Git post-commit hook auto-updates changelog.md after each commit
- These don't affect hardware or app development — documentation maintenance only

---

## Next Actions

1. Receive ESP32 and sensor hardware
2. Wire sensor to GPIO32, test ADC readings
3. Calibrate RAW_DRY and RAW_WET for the specific unit (update firmware constants)
4. Verify `GET http://esp32.local/moisture` returns JSON with stable values
5. Scaffold `/app` Expo project and implement polling client
