# STATE

## Snapshot
- Date: 2026-03-27
- Current milestone: M1 — First drivable core
- Current status: IN_PROGRESS
- Current gate: editor asset creation → PIE drivable loop
- Repository truth summary: UE 5.7 project compiles cleanly (24 source files, VS 2026 Community + MSVC 14.44), zero binary assets yet

## Last verified action
- Command or action: `Build.bat MileZeroEditor Win64 Development` via UE 5.7 UBT
- Result: **PASS — 4/4 actions succeeded, 0 errors, UnrealEditor-MileZero.dll linked**
- Evidence location: docs/VERIFICATION.md

## Current blockers / risks
- No hard blockers. Toolchain is fully operational.
- Risk: editor binary assets (maps, input actions, skeletal meshes) still need to be created in-editor or via automation.

## Files changed in this session
- Source/MileZero/MileZero.Build.cs (fixed: V6 include paths + conditional UnrealEd dependency)
- Source/MileZero/Vehicles/MZVehiclePawn.h/.cpp (fixed: API calls + added BootstrapDefaultInput)
- Source/MileZero/Setup/MZSetupCommandlet.h/.cpp (created: auto-generates test level)
- Content/MileZero/Maps/Test/L_MZ_TestTrack.umap (created via commandlet)
- Content/Vehicles/ (copied: OffroadCar + SportsCar template assets)
- Config/DefaultEngine.ini (fixed: removed invalid DefaultGraphicsRHI)
- Config/DefaultGame.ini (fixed: ProjectID → GUID format)
- docs/ (all updated: STATE, TODO, VERIFICATION, MILESTONE_STATUS, KNOWN_ISSUES, EDITOR_BOOTSTRAP)

## Next immediate actions
1. Open editor: `tools\open_editor.bat`
2. Create `BP_MZVehicle_Hatch` Blueprint from `MZVehiclePawn` class
3. Assign `SKM_Offroad` or `SKM_SportsCar` skeletal mesh + configure wheel setups
4. Set GameMode default pawn to the new Blueprint
5. PIE test in L_MZ_TestTrack

## Notes for next session
- UE 5.7 + VS 2026 Community (18 Insiders) with MSVC 14.44 confirmed working
- Build settings: BuildSettingsVersion.V6, EngineIncludeOrderVersion.Unreal5_7
- Input actions + mapping context auto-created at runtime (no editor asset setup needed)
- Test level created via MZSetup commandlet with floor, PlayerStart, lights
- Only remaining M1 blocker: vehicle needs skeletal mesh assigned in a Blueprint
