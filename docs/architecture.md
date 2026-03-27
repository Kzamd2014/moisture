# Architecture: moisture

## System Overview

Data flows one direction: **Sensor → ESP32 → endpoint → app**

No broker, no cloud, no persistence.

---

## Layers

### Hardware Layer
- **Sensor:** Generic Capacitive Soil Moisture Sensor v1.2 (analog output)
- **MCU:** ESP32
- **Wiring:** Sensor signal wire → ADC1 pin (GPIO32–39)

### Firmware Layer (Arduino C++)
- Reads analog output from sensor via ADC1
- Maps raw ADC value to 0–100% moisture:

```cpp
const int RAW_DRY = 2900;  // sensor in air
const int RAW_WET  = 1200;  // sensor submerged in water

int moisture = map(rawADC, RAW_DRY, RAW_WET, 0, 100);
moisture = constrain(moisture, 0, 100);
```

- Runs HTTP server; serves `GET /moisture` endpoint
- Advertises via mDNS as `esp32.local`
- Phase 2: also advertises BLE GATT characteristic with moisture value

### Transport Layer

**Phase 1 — Wi-Fi / HTTP REST**
- ESP32 hosts HTTP server on local network
- Client polls `GET http://esp32.local/moisture` every 10 seconds
- Response: `{"moisture": 42}`

**Phase 2 — BLE Notify (added alongside HTTP, no removal)**
- ESP32 advertises BLE GATT characteristic
- App subscribes to notifications; ESP32 pushes on value change
- BLE eliminates polling on the app side; HTTP endpoint remains active for debugging or alternative clients

### Mobile App Layer (React Native / Expo)
- **Phase 1:** Expo managed workflow — polls HTTP endpoint, renders moisture %
- **Phase 2:** Ejected to bare workflow — adds `react-native-ble-plx` for BLE notifications

---

## Data Flow

```
Capacitive Sensor
       │ analog signal
       ▼
  ESP32 ADC1
       │ map to 0–100%
       ├── Phase 1 ──→ HTTP GET /moisture ──→ Expo app (polls 10s)
       └── Phase 2 ──→ BLE characteristic ──→ React Native app (notify)
```

---

## Project Structure

```
/firmware   ESP32 Arduino sketch
/app        React Native (Expo) mobile app
/docs       Documentation and planning notes
```

---

## Key Design Decisions

| Decision | Choice | Rationale |
|---|---|---|
| Sensor pin | ADC1 (GPIO32–39) | ADC2 conflicts with Wi-Fi |
| Phase 2 approach | Eject to bare workflow | `react-native-ble-plx` requires it |
| Transport coexistence | Both HTTP and BLE active in Phase 2 | Learning comparison, no removal |
| mDNS | `esp32.local` | Avoids hardcoding IP; Android fallback available |
