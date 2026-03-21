# TODO

## Now (BLOCKED — requires user action)
- [ ] **USER ACTION**: install Epic Games Launcher + Unreal Engine 5.5 + Visual Studio 2022 (see docs/BUILD_RUNBOOK.md for exact steps)
- [ ] run `tools\generate_project_files.bat`
- [ ] run `tools\compile_check.bat` — fix any compile errors per docs/KNOWN_ISSUES.md
- [ ] open editor, run `tools/setup_editor_assets.py` or follow docs/EDITOR_BOOTSTRAP.md
- [ ] set up vehicle mesh + wheel bones on BP wrapper or default pawn
- [ ] PIE test in L_MZ_TestTrack → verify M1 gates

## Next (after M0+M1 verified)
- [ ] wire UMZSurfaceContactComponent to vehicle pawn
- [ ] verify dry asphalt vs gravel difference in feel (M2 first gate)
- [ ] expose telemetry (speed, RPM, gear, surface) via HUD or debug print (M2 second gate)

## Later
- [ ] create DA_MZVehicle_Coupe and DA_MZVehicle_Pickup (M3)
- [ ] build telemetry HUD overlay WBP_MZTelemetry (M2 full)
- [ ] implement UMZVehicleDamageComponent (M4)
- [ ] build main world greybox (M5)

## Deferred
- [ ] on-foot character enter/exit vehicle flow (stubbed, clean interface exists)
- [ ] traffic AI (M7)
- [ ] day/night cycle (M7)
- [ ] save/load persistence (M6)
