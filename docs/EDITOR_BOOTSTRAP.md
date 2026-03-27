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
- **Vehicle skeletal mesh**: Chaos vehicle requires a skeletal mesh with wheel bones (KI-005)

---

## Step 1: Open the editor
```
tools\open_editor.bat
```
Or double-click `MileZero.uproject`. First launch compiles shaders — wait for it to finish.

## Step 2: Add vehicle content to the project
In the editor: **Edit > Plugins**, search for and enable:
- **ChaosVehiclesPlugin** (should already be enabled)

Then add vehicle template content:
1. **Content Drawer** > **Add/Import** > **Add Feature or Content Pack**
2. Select the **Vehicle** template content
3. This adds skeletal meshes with proper wheel bones

Alternatively, the vehicle content from the template has been pre-copied to `Content/Vehicles/OffroadCar/` and `Content/Vehicles/SportsCar/`. If references work, use `SKM_Offroad` or `SKM_SportsCar` directly.

## Step 3: Create a vehicle Blueprint
1. In Content Browser, go to `Content/MileZero/Blueprints/Vehicles/`
2. Right-click > Blueprint Class > search `MZVehiclePawn` > Create
3. Name: `BP_MZVehicle_Hatch`
4. Open the Blueprint, select the **Mesh** component (inherited from WheeledVehiclePawn)
5. In Details, set **Skeletal Mesh** to `SKM_Offroad` (or `SKM_SportsCar`)
6. Configure the **ChaosWheeledVehicleMovementComponent**:
   - Set up 4 wheels (FL, FR, RL, RR) matching the skeleton bones
   - Front wheels: `Wheel_Front_Left`, `Wheel_Front_Right`
   - Rear wheels: `Wheel_Rear_Left`, `Wheel_Rear_Right`
7. Compile and Save the Blueprint

## Step 4: Set GameMode to use the Blueprint pawn
Option A (per-level): In the test level's **World Settings** > **GameMode Override** > set **Default Pawn Class** to `BP_MZVehicle_Hatch`

Option B (global): In **Project Settings > Maps & Modes > Default Pawn Class**, set to `BP_MZVehicle_Hatch`

## Step 5: PIE test
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
