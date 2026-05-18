---
description: "Task list for MVP Wi-Fi HTTP Moisture Endpoint"
---

# Tasks: MVP Wi-Fi HTTP Moisture Endpoint

**Input**: Design documents from `specs/001-mvp-wifi-http/`

**Prerequisites**: plan.md ✅, spec.md ✅, research.md ✅, data-model.md ✅, contracts/get-moisture.md ✅

**Tests**: Not requested — manual browser verification per quickstart.md.

**Organization**: Tasks are grouped by user story to enable independent implementation and
testing of each story.

## Format: `[ID] [P?] [Story?] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2)
- Include exact file paths in descriptions

## Path Conventions

- Firmware sketch: `firmware/moisture/moisture.ino`
- Firmware docs: `firmware/README.md`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Create project skeleton and include required libraries.

- [x] T001 Create `firmware/moisture/` directory and scaffold `firmware/moisture/moisture.ino` with empty `setup()` and `loop()` functions
- [x] T002 Add `#include` directives for `WiFi.h`, `WebServer.h`, and `ESPmDNS.h` at the top of `firmware/moisture/moisture.ino`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Wi-Fi and mDNS must be established before HTTP responses can be served.

**⚠️ CRITICAL**: No user story work can begin until this phase is complete.

- [x] T003 Define `WIFI_SSID` and `WIFI_PASSWORD` constants and implement Wi-Fi connection loop in `setup()` in `firmware/moisture/moisture.ino` (print assigned IP to Serial at 115200 baud on connect)
- [x] T004 Initialise `ESPmDNS` and advertise `esp32.local` hostname in `setup()` after Wi-Fi connects in `firmware/moisture/moisture.ino`
- [x] T005 Instantiate `WebServer server(80)` and call `server.begin()` at the end of `setup()` in `firmware/moisture/moisture.ino`; add `server.handleClient()` to `loop()`

**Checkpoint**: Flash and verify serial monitor shows IP address and mDNS is initialised.

---

## Phase 3: User Story 1 — Retrieve Live Moisture Reading (Priority: P1) 🎯 MVP

**Goal**: Browser GET to `/moisture` returns valid JSON with `moisture_percent` in [0, 100].

**Independent Test**: Open browser → `http://esp32.local/moisture` → receive
`{"moisture_percent": N}` where N is an integer. Deliver complete MVP value.

### Implementation for User Story 1

- [x] T006 [US1] Define calibration constants `SENSOR_PIN` (default: 32), `RAW_DRY` (default: 2900), `RAW_WET` (default: 1200), and `SAMPLE_COUNT` (default: 10) as `const int` at the top of `firmware/moisture/moisture.ino` with inline comments explaining each
- [x] T007 [US1] Implement `readAveragedADC()` function that reads `SENSOR_PIN` `SAMPLE_COUNT` times and returns the integer average in `firmware/moisture/moisture.ino`
- [x] T008 [US1] Implement `toMoisturePercent(int raw)` function using `map(raw, RAW_DRY, RAW_WET, 0, 100)` and `constrain(..., 0, 100)` in `firmware/moisture/moisture.ino`
- [x] T009 [US1] Implement sensor fault detection in a `handleMoisture()` function: if raw value is 0 or ≥ 4090, return `{"moisture_percent": 0, "error": "sensor fault: raw value out of range (N)"}` in `firmware/moisture/moisture.ino`
- [x] T010 [US1] Complete `handleMoisture()` happy path: call `readAveragedADC()` → `toMoisturePercent()` → respond with `{"moisture_percent": N}` and `Content-Type: application/json` in `firmware/moisture/moisture.ino`
- [x] T011 [US1] Register `handleMoisture` as the handler for `GET /moisture` via `server.on("/moisture", HTTP_GET, handleMoisture)` in `setup()` in `firmware/moisture/moisture.ino`

**Checkpoint**: Flash → open browser → `http://esp32.local/moisture` returns valid JSON.
User Story 1 is fully functional and independently testable at this point.

---

## Phase 4: User Story 2 — Consistent Reading Under Stable Conditions (Priority: P2)

**Goal**: Five consecutive readings with sensor stationary fall within a ±5 percentage point band.

**Independent Test**: Make 5 GET requests with sensor in fixed position; compare all five
`moisture_percent` values — all should be within 5 of each other.

### Implementation for User Story 2

- [ ] T012 [US2] Manually perform 5 consecutive GET requests with sensor stationary and record readings; if spread > 5%, increase `SAMPLE_COUNT` to 20 and reflash `firmware/moisture/moisture.ino`

**Checkpoint**: All five readings within ±5 pp. User Stories 1 and 2 both independently pass.

---

## Phase 5: Polish & Cross-Cutting Concerns

**Purpose**: Documentation and final validation.

- [x] T013 [P] Create `firmware/README.md` covering: wiring diagram reference (`docs/hardware_setup.md`), Arduino IDE board/port selection, serial monitor baud rate (115200), IP fallback for Android, calibration update procedure
- [ ] T014 Run through every step in `specs/001-mvp-wifi-http/quickstart.md` with the physical device and confirm each step succeeds; update quickstart if any step is inaccurate

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies — start immediately
- **Foundational (Phase 2)**: Depends on Phase 1 — blocks all user stories
- **User Story 1 (Phase 3)**: Depends on Phase 2 — T006–T011 can proceed in order
- **User Story 2 (Phase 4)**: Depends on Phase 3 being complete (needs working endpoint)
- **Polish (Phase 5)**: Depends on both user stories passing their checkpoints

### Within User Story 1

```
T006 (constants) → T007 (ADC read) → T008 (map fn) → T009+T010 (handler) → T011 (register route)
```

### Parallel Opportunities

```bash
# Phase 5 — documentation can be drafted while doing US2 validation:
Task: "Create firmware/README.md"           # T013 [P]
Task: "Validate quickstart.md steps"        # T014 (after hardware ready)
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (Wi-Fi + mDNS + WebServer)
3. Complete Phase 3: User Story 1 (T006–T011)
4. **STOP and VALIDATE**: Browser GET returns valid JSON — MVP done
5. Continue to Phase 4 once MVP is confirmed

### Incremental Delivery

1. Phases 1–2: Foundation ready (Wi-Fi, mDNS, WebServer)
2. Phase 3: Add moisture endpoint → validate in browser → **MVP achieved**
3. Phase 4: Add averaging tuning → validate stability → **V1 prerequisite met**
4. Phase 5: Polish and document

---

## Notes

- All tasks target a single file (`firmware/moisture/moisture.ino`) — sequential execution within each phase
- T013 is the only task marked [P] — it produces a different file (README) and can overlap with T012
- T012 is a manual validation step; no code change needed unless SAMPLE_COUNT requires tuning
- Calibration constants (T006) use defaults from `data-model.md`; update after running procedure in `docs/hardware_setup.md`
