# Brainstorm: Resolve Open Technical Questions

**Date:** 2026-03-23
**Status:** Complete
**Next step:** `/ce:plan` for MVP firmware

---

## What We're Building

Four pre-implementation technical decisions for the moisture sensor project, resolved before purchasing hardware or writing firmware.

---

## Resolved Questions

### 1. Sensor selection and ADC range

**Decision:** Generic Capacitive Soil Moisture Sensor v1.2 (common blue board, ~$2–5 on Amazon/AliExpress)

**Rationale:** Analog output directly to ESP32 ADC pin. Heavily documented with well-established calibration ranges. Lowest friction for a first build.

**ADC range (ESP32 12-bit, 0–4095):**
- `RAW_DRY` ≈ 2900 (sensor in air)
- `RAW_WET` ≈ 1200 (sensor submerged in water)

**Mapping logic:**
```cpp
// Constants — calibrate to your specific unit after delivery
const int RAW_DRY = 2900;
const int RAW_WET  = 1200;

int moisture = map(rawADC, RAW_DRY, RAW_WET, 0, 100);
moisture = constrain(moisture, 0, 100);
```

**Hardware wiring note:** When Wi-Fi is active, the ESP32 co-opts ADC2 internally, making GPIO0, 2, 4, 12–15, and 25–27 unavailable for analog reads. Wire the sensor to an **ADC1 pin (GPIO32–39)**. GPIO32 or GPIO34 are typical choices.

**Action required after hardware arrives:** Measure actual dry and wet ADC values and update constants.

---

### 2. Polling interval

**Decision:** 10 seconds

**Rationale:** Soil moisture changes on the order of minutes to hours — 10s gives a responsive feel in the app without taxing the ESP32. A good middle ground between 5s (slightly aggressive) and 30s (perceptibly sluggish for a demo).

---

### 3. ESP32 IP address strategy

**Decision:** mDNS — ESP32 advertises as `esp32.local`

**Rationale:** No hardcoding, no router config required. Clean dev experience. App polls `http://esp32.local/moisture`.

**Implementation note:** ESP32 firmware uses the `ESPmDNS` library (bundled with Arduino ESP32 core). App uses the hostname directly — no IP address management needed.

**Known limitation:** mDNS can be flaky on some Android devices. If the app fails to resolve `esp32.local`, fall back to hardcoding the IP temporarily during debugging.

---

### 4. BLE library / Expo workflow

**Decision:** Eject to Expo bare workflow for Phase 2

**Rationale:** Expo managed workflow cannot support BLE in Expo Go, requiring a custom dev build anyway. Bare workflow gives full native access without workarounds, and avoids build surprises mid-project.

**Timing:** Stay in managed workflow through MVP and V1 (Wi-Fi only). Eject to bare when starting Phase 2 (BLE). Update `CLAUDE.md` stack table at that point.

**BLE library:** `react-native-ble-plx` (most actively maintained React Native BLE library, works in bare workflow).

---

## Key Decisions Summary

| Question | Decision |
|---|---|
| Sensor | Generic Capacitive v1.2 |
| ADC dry value | ~2900 (calibrate post-delivery) |
| ADC wet value | ~1200 (calibrate post-delivery) |
| Polling interval | 10 seconds |
| IP strategy | mDNS (`esp32.local`) |
| BLE approach | Eject to bare workflow at Phase 2 start |

---

## Open Questions

None — all pre-implementation blockers resolved.

---

## Next Steps

1. Purchase hardware: ESP32 dev board + Generic Capacitive Soil Moisture Sensor v1.2
2. Run `/ce:plan` for MVP milestone (firmware + HTTP server)
3. After hardware arrives: calibrate `RAW_DRY` and `RAW_WET` constants with actual readings
