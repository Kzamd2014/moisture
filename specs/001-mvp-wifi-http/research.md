# Research: MVP Wi-Fi HTTP Moisture Endpoint

**Feature**: `001-mvp-wifi-http`
**Date**: 2026-05-17

## Decision 1: ADC Reading Strategy

**Decision**: Average 10 ADC samples per reading.

**Rationale**: The ESP32 ADC is notoriously noisy, especially at the upper end of its range.
Capacitive soil moisture sensors output a slowly-changing analog voltage, so reading latency
is not a concern — stability is. Averaging 10 samples in firmware reduces noise enough to meet
the ±5 percentage point stability criterion (SC-003) without adding external components.

**Alternatives considered**:
- Single sample: Faster but exhibits visible jitter; fails SC-003 on its own.
- Median filter (5 or more samples): More robust against outliers but adds implementation
  complexity. YAGNI for MVP — averaging is simpler and sufficient.
- External RC filter on the ADC pin: Valid hardware approach; deferred to post-calibration
  if software averaging proves insufficient.

---

## Decision 2: mDNS Hostname Strategy

**Decision**: Advertise `esp32.local` using the built-in `ESPmDNS` library; document the
device's static/DHCP IP as a fallback in `quickstart.md`.

**Rationale**: `ESPmDNS` is included in the Arduino ESP32 core — zero additional dependencies.
It makes the endpoint human-readable (`http://esp32.local/moisture`) on macOS and Linux
browsers. The fallback IP is essential because mDNS is unreliable on Android and inconsistent
on some Windows configurations.

**Alternatives considered**:
- Static IP only: Simpler but requires knowing/setting the IP in router or firmware; less
  convenient for development.
- DNS server on ESP32: Overkill; introduces a second service and more code.
- No hostname (IP only): Works but degrades developer experience; `esp32.local` is trivially
  achievable.

**Known limitation**: mDNS does not work reliably on Android. The V1 mobile app will use the
hardcoded IP fallback as noted in CLAUDE.md.

---

## Decision 3: JSON Response Format

**Decision**: Hand-format a minimal JSON string directly in the firmware.

Normal case: `{"moisture_percent": 42}`
Error case: `{"moisture_percent": 0, "error": "sensor fault: raw value out of range"}`

**Rationale**: The response contains at most two fields. Using the `ArduinoJson` library for
one or two fields is unnecessary complexity (Principle III). A `snprintf` or `String`
concatenation in the handler is readable, testable, and adds no dependencies.

**Alternatives considered**:
- `ArduinoJson` library: Robust and type-safe, but adds a dependency and compilation weight
  for a payload this simple. Deferred to V2 if BLE characteristic format becomes complex.
- Custom binary format: Not machine-readable from a browser; rejected.

---

## Decision 4: HTTP Server Library

**Decision**: Use the built-in `WebServer` class from the Arduino ESP32 core.

**Rationale**: No additional library needed. `WebServer` handles `GET /moisture` with a
callback pattern and sets `Content-Type: application/json` trivially. Sufficient for a single
endpoint with no auth, no body parsing, and no concurrency requirements.

**Alternatives considered**:
- `ESPAsyncWebServer`: More feature-rich (true async, chunked responses), but adds a library
  dependency and complexity not needed for a single synchronous GET handler.
- Raw TCP socket: Too low-level; no benefit for this use case.

---

## Decision 5: Calibration Constant Management

**Decision**: Define `RAW_DRY` and `RAW_WET` as `const int` values at the top of the sketch
with explanatory inline comments. Default values: `RAW_DRY = 2900`, `RAW_WET = 1200`.
Update after hardware delivery and manual calibration per the procedure in `hardware_setup.md`.

**Rationale**: The mapping from raw ADC to percentage is linear: 
`moisture_percent = map(raw, RAW_DRY, RAW_WET, 0, 100)` clamped to [0, 100].
Keeping constants at the top of the file makes them trivially findable and editable without
understanding the rest of the code (Principle IV).

**Alternatives considered**:
- Runtime calibration via a second endpoint: Adds scope; calibration is a one-time setup step
  per sensor unit, not a runtime concern.
- Non-volatile storage (NVS/EEPROM): Unnecessary for a single developer who can reflash.
