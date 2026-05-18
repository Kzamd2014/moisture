# Feature Specification: MVP Wi-Fi HTTP Moisture Endpoint

**Feature Branch**: `001-mvp-wifi-http`

**Created**: 2026-05-17

**Status**: Draft

**Input**: User description: "MVP Wi-Fi HTTP endpoint — ESP32 reads capacitive soil moisture sensor ADC value, maps it to 0-100% moisture percentage, and serves it via GET /moisture returning JSON. Done when a browser GET returns a valid JSON moisture reading."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Retrieve Live Moisture Reading (Priority: P1)

The developer opens a web browser on any device connected to the same local Wi-Fi network,
navigates to the device's address, and receives the current soil moisture level as a percentage
in a structured data format. No apps, no accounts, no setup beyond the device being powered on.

**Why this priority**: This is the entire MVP — without a working endpoint returning a valid
reading, nothing else can be built or verified.

**Independent Test**: Navigate to `http://esp32.local/moisture` (or device IP) in a browser.
A valid JSON response containing a moisture percentage delivers complete value on its own.

**Acceptance Scenarios**:

1. **Given** the device is powered and connected to Wi-Fi, **When** the developer sends a GET
   request to `/moisture`, **Then** the response is valid JSON containing a `moisture_percent`
   field with a numeric value between 0 and 100.

2. **Given** the sensor is submerged in water, **When** the developer sends a GET request,
   **Then** `moisture_percent` is near 100 (high reading reflects wet conditions).

3. **Given** the sensor is in dry air, **When** the developer sends a GET request,
   **Then** `moisture_percent` is near 0 (low reading reflects dry conditions).

---

### User Story 2 - Consistent Reading Under Stable Conditions (Priority: P2)

When the sensor is left in the same medium (e.g., dry soil) and the developer polls the
endpoint multiple times, the returned readings are stable and do not fluctuate wildly between
requests.

**Why this priority**: Unstable readings make the endpoint unusable for the mobile app polling
phase. Stability must be confirmed at the MVP level.

**Independent Test**: Make 5 consecutive GET requests with the sensor in the same position.
All five `moisture_percent` values should fall within a ±5% band.

**Acceptance Scenarios**:

1. **Given** the sensor is stationary in soil, **When** the developer makes 5 GET requests
   within 30 seconds, **Then** all five `moisture_percent` values are within 5 percentage
   points of each other.

---

### Edge Cases

- What happens when the raw sensor value falls outside the calibrated dry/wet range?
  Response MUST still return a valid JSON body; `moisture_percent` is clamped to 0 or 100.
- What if the device loses Wi-Fi? Connection simply fails; no response is expected while offline.
- What if the sensor is disconnected or returns an implausible value?
  Response MUST include an `error` field describing the fault; `moisture_percent` MAY be omitted.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST expose a network endpoint that returns the current soil moisture level.
- **FR-002**: Moisture level MUST be expressed as a percentage value between 0 and 100 (inclusive).
- **FR-003**: Response MUST be in a structured, machine-readable format containing at minimum a
  moisture percentage field.
- **FR-004**: The endpoint MUST be accessible from any browser on the same local network without
  authentication or special tooling.
- **FR-005**: The device MUST be reachable by a human-readable local hostname as well as by IP address.
- **FR-006**: Raw sensor values MUST be mapped to the 0–100 range using defined calibration
  constants; values outside the calibrated range MUST be clamped, not rejected.
- **FR-007**: When a sensor fault is detected, the response MUST include a machine-readable error
  indicator alongside or instead of the moisture percentage.

### Key Entities

- **MoistureReading**: Represents a single sensor sample.
  Fields: `moisture_percent` (integer 0–100), optional `error` (string describing fault condition).
- **CalibrationConstants**: The dry and wet ADC boundary values used to compute the percentage.
  These are set at firmware build time and documented in source; they are not exposed via the endpoint.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: A moisture reading is returned within 3 seconds of the developer making a request,
  measured on the same local network.
- **SC-002**: Returned percentage is within the 0–100 range for all sensor conditions tested
  (dry air, wet soil, submerged).
- **SC-003**: Five consecutive readings under stable sensor conditions fall within a ±5 percentage
  point band.
- **SC-004**: No additional software beyond a standard web browser is required to retrieve and
  read a moisture value.
- **SC-005**: Endpoint remains available and responsive for at least 30 minutes of continuous
  operation without a reboot.

## Assumptions

- Developer and device are on the same local Wi-Fi network during testing.
- mDNS hostname resolution is available on the developer's browser/OS; a hardcoded IP address
  serves as fallback if mDNS fails.
- Calibration constants (dry and wet ADC boundary values) will be tuned after hardware delivery;
  the default values in the spec (RAW_DRY ≈ 2900, RAW_WET ≈ 1200) are placeholders.
- No security or authentication requirements apply — this is a single-developer local network setup.
- The device has a stable Wi-Fi connection; connectivity issues are outside this feature's scope.
- Only one sensor is in scope for the MVP; multi-sensor support is explicitly out of scope.
