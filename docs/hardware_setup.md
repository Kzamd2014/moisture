# Hardware Setup & Calibration

## Wiring

| Sensor pin | ESP32 pin |
|---|---|
| VCC | 3.3V or 5V |
| GND | GND |
| AOUT (signal) | Any ADC1 pin (GPIO32–39) |

**Important:** Do not use ADC2 pins (GPIO0, 2, 4, 12–15, 25–27) — they are unavailable when Wi-Fi is active.

---

## ADC Calibration

The sensor outputs a raw ADC value that must be mapped to a 0–100% moisture percentage. The mapping constants depend on your specific unit — measure them after hardware arrives.

### How to calibrate

1. **Dry reading (RAW_DRY):** Hold the sensor in open air. Record the raw ADC value — expected ~2900.
2. **Wet reading (RAW_WET):** Submerge the sensor tip in water. Record the raw ADC value — expected ~1200.
3. Update the constants in the firmware sketch:

```cpp
const int RAW_DRY = 2900;  // sensor in air — replace with your measured value
const int RAW_WET  = 1200;  // sensor submerged in water — replace with your measured value
```

The mapping function clamps output to 0–100:

```cpp
int moisture = map(rawADC, RAW_DRY, RAW_WET, 0, 100);
moisture = constrain(moisture, 0, 100);
```

### Notes
- Raw values decrease as moisture increases (wetter = lower ADC reading)
- `constrain()` prevents out-of-range values if the sensor is bone dry or fully submerged
- Re-calibrate if you switch to a different sensor unit

---

## Verification

Once wired and calibrated, confirm with a serial monitor or browser:

```
GET http://esp32.local/moisture
→ {"moisture": 42}
```

A stable reading in open air should be near 0%; a reading with the tip in water near 100%.
