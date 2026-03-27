# EDITOR_BOOTSTRAP

Exact step-by-step procedure to get from "toolchain installed" to "first drivable loop in PIE."

## Prerequisites
- Unreal Engine 5.7 installed via Epic Games Launcher
- Visual Studio 2026 Community (or 2022+) with C++ Desktop workload + Windows SDK
- MileZero.uproject associated with UE 5.7
- **Already done**: `tools\compile_check.bat` passed, `tools\run_setup.bat` created test level

---

## Current state (2026-03-27)
- Code compiles cleanly on UE 5.7 + VS 2026
- Test level `L_MZ_TestTrack` auto-generated with floor, PlayerStart, lights
- Vehicle input bootstrap: auto-creates input actions + mapping context if none assigned in editor
- Vehicle template content (OffroadCar, SportsCar) copied to `Content/Vehicles/`
- Config: GlobalDefaultGameMode = MZGameMode, default map = L_MZ_TestTrack

## What works without any editor setup
- Vehicle spawns at PlayerStart (MZGameMode sets DefaultPawnClass = MZVehiclePawn)
- Input actions auto-created: W/S=throttle/brake, A/D=steer, Space=handbrake, C=camera, R=reset
- Chase camera + hood camera cycling

## What still needs editor work
- **Nothing critical** — vehicle auto-loads SKM_Offroad mesh + configures 4 wheels in C++ constructor
- **If mesh fails to load**: add Vehicle content pack from editor ("Add Feature or Content Pack" > Vehicle)

---

## Step 1: Open the editor
```
tools\open_editor.bat
```
Or double-click `MileZero.uproject`. First launch compiles shaders — wait for it to finish.

## Step 2: PIE test (no Blueprint needed — fully self-configuring in C++)
1. Open `L_MZ_TestTrack` (should auto-load)
2. Press **Alt+P** (Play In Editor)
3. Verify:
   - [ ] Vehicle spawns at PlayerStart
   - [ ] W = throttle (vehicle moves forward)
   - [ ] S = brake
   - [ ] A/D = steer
   - [ ] Space = handbrake
   - [ ] C = camera cycle (chase → hood → chase)
   - [ ] R = reset vehicle to spawn
   - [ ] Mouse = look around (chase cam)
4. If all pass → **M1 is VERIFIED**

## Step 6: Log results
Update these docs after PIE test:
- docs/VERIFICATION.md — add test results
- docs/MILESTONE_STATUS.md — M1 → VERIFIED
- docs/STATE.md — advance to M2
