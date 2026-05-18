# Contract: GET /moisture

**Feature**: `001-mvp-wifi-http`
**Date**: 2026-05-17

## Endpoint

```
GET http://esp32.local/moisture
```

Fallback (if mDNS unavailable):
```
GET http://<device-ip>/moisture
```

## Request

No parameters, no headers, no authentication required.

## Response

**Content-Type**: `application/json`

**HTTP Status Codes**:

| Code | Condition |
|------|-----------|
| 200 | Reading obtained (normal or clamped) |
| 200 | Sensor fault detected (error field present) |

> The device always returns HTTP 200. Fault conditions are indicated by the `error` field
> in the JSON body, not by HTTP status codes. This keeps client logic simple for the MVP.

### Normal Response Body

```json
{
  "moisture_percent": 42
}
```

### Fault Response Body

```json
{
  "moisture_percent": 0,
  "error": "sensor fault: raw value out of range (4095)"
}
```

## Field Definitions

| Field | Type | Always Present | Description |
|-------|------|---------------|-------------|
| `moisture_percent` | integer (0–100) | Yes | Soil moisture percentage. Clamped to [0, 100]. |
| `error` | string | No | Fault description. Only present on sensor fault. |

## Behaviour Guarantees

- Response body is always valid JSON.
- `moisture_percent` is always an integer in [0, 100].
- Response is available on both `esp32.local` (mDNS) and the device's IP address.
- No CORS headers are set (not needed for single-developer local use).
- No rate limiting (10-second poll interval is enforced by the client, not the server).

## Out of Scope for MVP

- Authentication or API keys
- HTTPS / TLS
- Multiple endpoints
- Query parameters (e.g., units, format)
- HTTP status codes for error conditions
