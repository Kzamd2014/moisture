<!--
SYNC IMPACT REPORT
==================
Version change: (template) → 1.0.0
Modified principles: N/A (initial ratification)
Added sections: Core Principles (I–IV), Governance
Removed sections: all placeholder tokens replaced
Templates updated:
  ✅ .specify/templates/plan-template.md — Constitution Check gates align with principles below
  ✅ .specify/templates/spec-template.md — no mandatory section changes required
  ✅ .specify/templates/tasks-template.md — path conventions align with /firmware + /app layout
Deferred TODOs: none
-->

# moisture Constitution

## Core Principles

### I. Hardware Constraints First

The sensor and microcontroller layer is the source of truth. Software MUST adapt to hardware
limitations — never the other way around. Specifically:

- Firmware MUST use ADC1 pins (GPIO32–39) only; ADC2 is unavailable when Wi-Fi is active
- BLE and Wi-Fi MAY coexist; neither transport MUST be removed when the other is added
- ADC calibration constants (RAW_DRY, RAW_WET) MUST be documented in firmware source with inline comments
- Any change that requires rewiring or re-flashing hardware MUST be called out explicitly in the commit message

### II. Phased Progression

Features MUST be built in the defined phase order: MVP (Wi-Fi HTTP) → V1 (Mobile app) → V2 (BLE).
Skipping a phase is not permitted. Specifically:

- Each phase MUST reach its "Done when" criterion before the next phase begins
- Phase V2 MUST preserve the Phase MVP HTTP transport; removal of existing transports is out of scope
- New transports or UI layers MUST be additive, not replacements

### III. Radical Simplicity

This is a solo learning project with no production deployment. Complexity MUST be justified
against actual learning value. Specifically:

- Data flows in one direction only: Sensor → ESP32 → endpoint → app
- No cloud backend, no message broker, no data persistence, no push notifications
- No multi-sensor, multi-plant, or multi-user support
- If a simpler approach delivers the same learning outcome, the simpler approach MUST be chosen
- Any abstraction that exists only for hypothetical future requirements MUST be removed

### IV. Learning Transparency

Because the primary goal is to understand the hardware-to-mobile pipeline, decisions and
trade-offs MUST be made explicit. Specifically:

- ADC-to-percentage mapping constants MUST be commented in firmware
- Phase V2 MUST include dev notes comparing Wi-Fi vs BLE approaches
- Firmware and app changes MUST be committed separately to keep history readable
- Firmware lives in `/firmware`, mobile app in `/app` — these directories MUST not be merged

## Development Workflow

- Solo developer (Kelly); no PR review process required
- Commit firmware and app changes separately
- No approval required before committing or pushing to GitHub
- Constitution MUST be re-consulted at the start of each new phase

## Governance

This constitution supersedes all informal conventions. Amendments require:

1. A clear rationale tied to a specific hardware or project constraint
2. An update to `LAST_AMENDED_DATE` and a version bump per semver rules:
   - MAJOR: removal or redefinition of a principle
   - MINOR: new principle or section added
   - PATCH: wording clarification or typo fix
3. A follow-up review of any open spec or plan files for consistency

All specs and plans MUST include a Constitution Check section that gates on Principles I–IV
before implementation begins.

**Version**: 1.0.0 | **Ratified**: 2026-05-17 | **Last Amended**: 2026-05-17
