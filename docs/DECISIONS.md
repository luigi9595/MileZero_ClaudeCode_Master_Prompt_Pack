# DECISIONS

| Date | ID | Decision | Why | Consequence |
|---|---|---|---|---|
| 2026-03-16 | D-001 | Unreal Engine 5.x with C++-first architecture | matches project goals and technical contract | Blueprint use stays selective |
| 2026-03-16 | D-002 | Compact dense world over giant map | better replay value and faster milestone progress | world scope remains constrained |
| 2026-03-16 | D-003 | Data-driven tuning for vehicles, surfaces, damage, and activities | easier tuning, testing, and continuation | more up-front data asset work |
| 2026-03-16 | D-004 | M1 player spawns directly inside vehicle (skip on-foot) | unblocks first drivable loop immediately | on-foot enter/exit deferred, clean interface stubbed |
| 2026-03-16 | D-005 | ChaosWheeledVehicleMovementComponent as primary vehicle movement | contract requires Chaos vehicles; direct API avoids plugin fragmentation | tuning must be data-driven to avoid template-default feel |
| 2026-03-16 | D-006 | UPrimaryDataAsset for vehicles, surfaces, activities | enables Asset Manager scanning, future registry, clean data separation | editor must create data assets per EDITOR_BOOTSTRAP |
| 2026-03-16 | D-007 | Tool-limited fallback: full text-side scaffolding first | no UE5 or VS on current machine; don't stall | compile verification deferred until engine installed |
