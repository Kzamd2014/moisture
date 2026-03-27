# Project Status

**Last updated:** 2026-03-26
**Current phase:** Pre-implementation — hardware not yet delivered

---

## Milestone Checklist

### MVP — Sensor reads over Wi-Fi
- [ ] Capacitive sensor wired to ESP32 ADC1 pin (GPIO32–39)
- [ ] Raw ADC mapped to 0–100% moisture percentage
- [ ] ESP32 HTTP server responds to `GET /moisture` with JSON
- **Status:** Not started — awaiting hardware

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

## Infrastructure Notes

- Claude Code PostToolUse hook auto-updates project_status, architecture, and project_spec when CLAUDE.md or firmware/app files change
- Git post-commit hook auto-updates changelog.md after each commit
- These don't affect hardware or app development — documentation maintenance only

---

## Next Actions

1. Wire sensor to ESP32 once hardware arrives
2. Calibrate RAW_DRY and RAW_WET constants for the specific unit
3. Scaffold `/firmware` Arduino sketch
4. Scaffold `/app` Expo project
