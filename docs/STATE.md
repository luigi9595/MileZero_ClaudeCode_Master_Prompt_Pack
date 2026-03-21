# STATE

## Snapshot
- Date: 2026-03-16
- Current milestone: M0 — Project bootstrap and repo truth
- Current status: BLOCKED
- Current gate: compile verification — requires UE5 and Visual Studio
- Repository truth summary: complete text-side UE5 project skeleton (24 source files, 4 configs, 4 tool scripts, 1 Python automation script), zero binary assets

## Last verified action
- Command or action: exhaustive search for UE5 / VS / Epic Launcher on this machine
- Result: CONFIRMED NOT INSTALLED — no UE5, no VS, no Epic Launcher, no build tools, no registry entries
- Evidence location: docs/VERIFICATION.md

## Current blockers / risks
- Blocker 1: **No Unreal Engine 5.x anywhere on this machine** (checked: PATH, registry, C:/D:/E: drives, ProgramData, Epic manifests, vswhere)
- Blocker 2: **No Visual Studio or MSVC build tools** (checked: PATH, VS2019/2022 dirs, BuildTools dirs, vswhere)
- These are hard external blockers. No text-side workaround can advance past M0 compile gate.

## Files changed in this session
- Source/MileZero/Debug/MZDebugSubsystem.h/.cpp (fixed: UFUNCTION(Exec) → IConsoleManager registration)
- tools/setup_editor_assets.py (created: automated first-boot asset creation)
- docs/KNOWN_ISSUES.md (updated: 5 compile/runtime risks documented)
- docs/EDITOR_BOOTSTRAP.md (updated: exact click-by-click first-boot guide)
- docs/STATE.md (updated: BLOCKED status)
- docs/VERIFICATION.md (updated)
- docs/MILESTONE_STATUS.md (updated)
- docs/BUILD_RUNBOOK.md (updated)

## Next immediate actions
1. **YOU**: Install Epic Games Launcher → Install Unreal Engine 5.5 → Install Visual Studio 2022 with "Game development with C++" workload
2. Then run: `tools\generate_project_files.bat` or right-click MileZero.uproject → "Generate Visual Studio project files"
3. Then run: `tools\compile_check.bat`
4. Then run: `tools\setup_editor_assets.py` in the editor's Python console, or follow docs/EDITOR_BOOTSTRAP.md manually

## Notes for next session
- MZDebugSubsystem was fixed: console commands now use IConsoleManager (MZ.ResetVehicle, MZ.SpawnVehicle, MZ.Teleport, MZ.Telemetry)
- 5 compile/runtime risks documented in KNOWN_ISSUES.md — all are targeted one-line fixes
- setup_editor_assets.py will create input actions, mapping context, vehicle data asset, surface data assets, and test map on first run
- The gap to M1 is: install toolchain → compile → create assets → PIE test
