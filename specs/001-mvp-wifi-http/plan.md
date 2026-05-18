# Implementation Plan: MVP Wi-Fi HTTP Moisture Endpoint

**Branch**: `001-mvp-wifi-http` | **Date**: 2026-05-17 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/001-mvp-wifi-http/spec.md`

## Summary

Implement firmware for the ESP32 that reads the capacitive soil moisture sensor on ADC1,
maps the raw ADC value to a 0–100% moisture percentage using defined calibration constants,
and exposes a single `GET /moisture` HTTP endpoint returning a JSON response. The device
advertises itself on the local network via mDNS (`esp32.local`) with a hardcoded IP fallback.
Done when a browser GET returns valid JSON with a moisture percentage.

## Technical Context

**Language/Version**: C++ (Arduino framework, ESP32 Arduino Core v2.x)

**Primary Dependencies**: Arduino ESP32 Core built-ins — `WebServer`, `ESPmDNS`, `WiFi`
(no external libraries required for MVP)

**Storage**: N/A

**Testing**: Manual — browser HTTP GET to `/moisture`; verify JSON response and reading range

**Target Platform**: ESP32 microcontroller (Xtensa LX6 dual-core, 3.3V logic, Arduino IDE or PlatformIO)

**Project Type**: Embedded firmware / single-endpoint HTTP service

**Performance Goals**: Response returned within 3 seconds on local LAN; readings stable within
±5 percentage points across 5 consecutive requests under static sensor conditions

**Constraints**:
- ADC1 only (GPIO32–39); ADC2 is unavailable when Wi-Fi is active
- Capacitive sensor output is analog; no I2C/SPI protocol involved
- Sensor wired to a single ADC1 pin (exact pin TBD during wiring — any of GPIO32–39)
- RAW_DRY ≈ 2900, RAW_WET ≈ 1200 (calibrate to actual unit after hardware delivery)

**Scale/Scope**: Single device, single sensor, single developer

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Gate | Status |
|-----------|------|--------|
| I. Hardware Constraints First | Firmware uses ADC1 pin only; RAW_DRY/RAW_WET constants defined with inline comments | ✅ PASS |
| II. Phased Progression | Scope is MVP only; no mobile app or BLE work included | ✅ PASS |
| III. Radical Simplicity | Single endpoint, no storage, no cloud, no broker, one-direction data flow | ✅ PASS |
| IV. Learning Transparency | Calibration constants documented in source; mDNS vs IP fallback noted in quickstart | ✅ PASS |

All gates pass. Proceeding to Phase 0.

*Post-design re-check (Phase 1)*: No design decisions introduced complexity violations.
No Complexity Tracking entries required.

## Project Structure

### Documentation (this feature)

```text
specs/001-mvp-wifi-http/
├── plan.md              # This file
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output
├── quickstart.md        # Phase 1 output
├── contracts/
│   └── get-moisture.md  # Phase 1 output
├── checklists/
│   └── requirements.md  # Spec quality checklist
└── tasks.md             # Phase 2 output (/speckit-tasks — not created here)
```

### Source Code (repository root)

```text
firmware/
├── moisture/
│   └── moisture.ino     # Main sketch: WiFi setup, ADC read, HTTP server, mDNS
└── README.md            # Wiring notes, flash instructions
```

**Structure Decision**: Single embedded firmware sketch in `/firmware/moisture/`. No `src/`
hierarchy needed — a single `.ino` file is idiomatic for an Arduino sketch of this scope.
The mobile app (`/app`) is out of scope for MVP and will be created in V1.
