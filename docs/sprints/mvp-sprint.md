# MVP Sprint

**Goal:** ESP32 reads sensor, serves `GET /moisture` returning JSON over Wi-Fi.
**Done when:** Browser `GET http://esp32.local/moisture` returns valid JSON moisture reading.
**Status:** Firmware implementation complete — awaiting hardware for field testing and calibration verification

---

## Sprint Backlog

### Phase 0: Pre-sprint (before hardware arrives)
- [x] Scaffold `/firmware/moisture.ino` skeleton
- [ ] Test Arduino IDE — flash blink sketch, confirm USB drivers + board selection work
- [ ] Scaffold `/app` with `npx create-expo-app app`

### Phase 1: Hardware bring-up (Days 1–2)
- [ ] Wire sensor: AOUT → GPIO32, VCC → 3.3V, GND → GND
- [x] Read raw ADC values in serial monitor (confirm stable output) — implemented in firmware
- [ ] Calibrate RAW_DRY (sensor in air) — 10 readings, average (awaiting hardware)
- [ ] Calibrate RAW_WET (sensor tip in water) — 10 readings, average (awaiting hardware)
- [x] Update constants in firmware, verify `map()`/`constrain()` outputs 0–100% in serial monitor

**Phase 1 done when:** Serial monitor shows stable ADC + moisture % continuously.

### Phase 2: Networking (Days 3–4)
- [x] Connect ESP32 to Wi-Fi (`WiFi.begin()`)
- [x] Start mDNS — `esp32.local` resolves on laptop (advertises via MDNS.begin())
- [x] Implement `WebServer` with `GET /moisture` returning `{"moisture_percent": XX}`
- [ ] Test from browser and `curl` (awaiting hardware)

**Phase 2 done when:** `curl http://esp32.local/moisture` returns `{"moisture": 42}`.

### Phase 3: Wrap-up (Day 5)
- [ ] Add comments to firmware (calibration constants, pin choice, measurement date)
- [ ] Update `/docs/hardware_setup.md` with actual wiring + calibration values
- [ ] Verify endpoint stable over 5+ minutes
- [ ] Commit firmware to git

---

## Blockers & Mitigations

| Risk | Mitigation |
|---|---|
| Hardware delayed | Scaffold firmware + app offline; mock ADC with hardcoded value |
| mDNS fails on Android | Fall back to hardcoded IP (document both paths) |
| ADC noise | Average 5–10 samples per reading |
| Arduino IDE / driver issues | Catch in Phase 0 before hardware arrives |

---

## Calibration Log

| Condition | Readings | Average | Date |
|---|---|---|---|
| Dry (air) | — | 2900 (firmware default) | — (awaiting field measurement) |
| Wet (water) | — | 1200 (firmware default) | — (awaiting field measurement) |

---

## Sprint Review

*(Fill in at end of sprint)*

- **Completed:** —
- **Not completed:** —
- **Surprises / learnings:** —
- **Ready for V1?** —
