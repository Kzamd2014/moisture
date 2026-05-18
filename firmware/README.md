# moisture — firmware

Arduino sketch for the ESP32 that reads a capacitive soil moisture sensor and
serves the reading as JSON over HTTP.

## Wiring

See `docs/hardware_setup.md` for the full wiring diagram.

Key constraint: connect the sensor's analog output (AOUT) to any **ADC1 pin (GPIO32–39)**.
ADC2 pins are unavailable when Wi-Fi is active.

## Configuration

Open `moisture/moisture.ino` and update the constants at the top:

```cpp
const char* WIFI_SSID     = "your-network-name";
const char* WIFI_PASSWORD = "your-password";
const int   SENSOR_PIN    = 32;   // any GPIO32–39
const int   RAW_DRY       = 2900; // calibrate after delivery
const int   RAW_WET       = 1200; // calibrate after delivery
```

See `docs/hardware_setup.md` for the calibration procedure to get accurate RAW_DRY / RAW_WET
values for your specific sensor unit.

## Flashing (Arduino IDE)

1. Install the **ESP32 Arduino Core** via Boards Manager if not already installed.
2. **Tools → Board → ESP32 Dev Module**
3. **Tools → Port → /dev/cu.usbserial-xxx** (macOS) or `COM#` (Windows)
4. Click **Upload**
5. Open Serial Monitor at **115200 baud** — the device prints its IP on connect.

## Verifying the endpoint

After flashing, from a browser on the same Wi-Fi network:

```
http://esp32.local/moisture
```

Expected response:

```json
{"moisture_percent": 42}
```

### Android / mDNS fallback

mDNS (`esp32.local`) does not work on Android. Use the device's IP address instead —
printed to the serial monitor on every boot:

```
http://192.168.1.xxx/moisture
```

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| `esp32.local` doesn't resolve | Use IP address (see serial monitor) |
| `moisture_percent` stuck at 0 or 100 | Re-run calibration in `docs/hardware_setup.md` |
| `"error": "sensor fault..."` in response | Check wiring; verify `SENSOR_PIN` matches physical connection |
| No response / can't connect | Check SSID/password; confirm device and client on same network |
| Readings jumpy | Increase `SAMPLE_COUNT` (try 20) and reflash |
