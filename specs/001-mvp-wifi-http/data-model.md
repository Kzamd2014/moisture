# Data Model: MVP Wi-Fi HTTP Moisture Endpoint

**Feature**: `001-mvp-wifi-http`
**Date**: 2026-05-17

## Entities

### MoistureReading

Represents a single sampled moisture measurement returned by the device.

| Field | Type | Range | Required | Description |
|-------|------|-------|----------|-------------|
| `moisture_percent` | integer | 0–100 | Yes (unless error) | Soil moisture as a percentage. 0 = dry, 100 = saturated. Clamped at boundaries. |
| `error` | string | — | No | Human-readable fault description. Present only when the raw sensor value is implausible or the sensor is unresponsive. |

**Validation rules**:
- `moisture_percent` MUST be clamped to [0, 100] before serialisation; raw values outside
  the calibrated range do not cause an error — they clamp.
- `error` is only included when a sensor fault is detected (e.g., raw ADC value is 0 or
  above the ADC ceiling, indicating a wiring fault or disconnected sensor).
- When `error` is present, `moisture_percent` MAY be included (set to 0) or omitted,
  depending on whether a plausible reading was obtained.

**Examples**:

Normal reading:
```json
{"moisture_percent": 42}
```

Sensor fault:
```json
{"moisture_percent": 0, "error": "sensor fault: raw value out of range (4095)"}
```

---

### CalibrationConstants

Not exposed via the API. Defined in firmware source at build time.

| Constant | Default Value | Description |
|----------|--------------|-------------|
| `RAW_DRY` | 2900 | ADC value when sensor is in dry air. Upper bound of the mapping range. |
| `RAW_WET` | 1200 | ADC value when sensor is submerged in water. Lower bound of the mapping range. |
| `SAMPLE_COUNT` | 10 | Number of ADC samples averaged per reading. |

**Calibration procedure**: Defined in `docs/hardware_setup.md`. Must be performed after
hardware delivery. Default values are estimates; actual values vary by sensor unit.

---

## Mapping Formula

```
moisture_percent = map(averaged_raw, RAW_DRY, RAW_WET, 0, 100)
```

Where `map()` is the Arduino linear interpolation function. Result is then clamped:

```
moisture_percent = constrain(moisture_percent, 0, 100)
```

This is the only transformation between raw ADC and the API response.
