# MILESTONE_STATUS

| Milestone | Status | Source Files | Key Classes | Next Action |
|---|---|---|---|---|
| M0 — Project bootstrap and repo truth | VERIFIED | Build system, module | MileZero module, targets | — |
| M1 — First drivable core | CODE_COMPLETE | 24 files | AMZVehiclePawn, AMZGameMode, AMZPlayerController | PIE test |
| M2 — Surface system and telemetry | CODE_COMPLETE | +8 files | UMZSurfaceContactComponent, SMZTelemetryWidget, SMZHUDWidget, AMZHUD | PIE verify grip diff |
| M3 — Vehicle trio differentiation | CODE_COMPLETE | +4 files | UMZVehicleRegistry, MZVehicleSelectionWidget | PIE test 3 vehicles |
| M4 — Mechanical damage consequences | CODE_COMPLETE | +4 files | UMZVehicleDamageComponent, UMZDamageDataAsset | PIE crash test |
| M5 — Main world greybox | CODE_COMPLETE | +8 files | AMZWorldZone, AMZSpawnPoint, UMZWorldManager, MZWorldSetupCommandlet | Run commandlet |
| M6 — Activities, HUD, and save/load | CODE_COMPLETE | +8 files | UMZActivityManager, AMZCheckpointActor, UMZSaveManager | PIE activity loop |
| M7 — Traffic, day/night, wet-world | CODE_COMPLETE | +8 files | UMZTrafficSubsystem, AMZTrafficVehicle, AMZTimeOfDayManager, AMZWeatherManager | PIE integration |
| M8 — Hardening | IN_PROGRESS | integration edits | VehiclePawn wiring | Compile + smoke test |

## Total source files: 72 (.h + .cpp + .cs)

## Rules
- Only one milestone should usually be IN_PROGRESS at a time.
- CODE_COMPLETE means all C++ is written but pending compile + PIE verification.
- A milestone can become VERIFIED only if the relevant gates are logged in docs/VERIFICATION.md.
