# MILESTONE_STATUS

| Milestone | Status | Current Gate | Last Evidence | Next Action | Owner |
|---|---|---|---|---|---|
| M0 — Project bootstrap and repo truth | BLOCKED | compile verification (no UE5/VS on machine) | 2026-03-16: full text-side scaffolding, code audit, automation script | install UE5 5.5 + VS2022, run compile_check.bat | User → Claude |
| M1 — First drivable core | NOT_STARTED | none | M1 source code pre-built, editor automation script ready | after M0: create assets → PIE test | Claude |
| M2 — Surface system and telemetry | NOT_STARTED | none | surface data asset + contact component written | wire to vehicle, build telemetry after M1 | Claude |
| M3 — Vehicle trio differentiation | NOT_STARTED | none | vehicle data asset class exists | create 3 data assets + tuning after M2 | Claude |
| M4 — Mechanical damage consequences | NOT_STARTED | none | none | build damage component and data after M3 | Claude |
| M5 — Main world greybox | NOT_STARTED | none | none | build compact route structure after M4 | Claude |
| M6 — Activities, HUD, and save/load | NOT_STARTED | none | activity data asset class exists | implement structured loop after M5 | Claude |
| M7 — Traffic, day/night, wet-world integration, activity completion | NOT_STARTED | none | none | integrate systemic world behavior after M6 | Claude |
| M8 — Hardening, performance, and release-candidate pass | NOT_STARTED | none | none | stabilize and document after M7 | Claude |

## Rules
- Only one milestone should usually be IN_PROGRESS at a time.
- A milestone can become VERIFIED only if the relevant gates are logged in docs/VERIFICATION.md.
- If a milestone is BLOCKED, write the blocker precisely and keep the repo improving through the fallback path.
