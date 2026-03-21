# EDITOR_BOOTSTRAP

Exact step-by-step procedure to get from "toolchain installed" to "first drivable loop in PIE."

## Prerequisites
- Unreal Engine 5.5 installed via Epic Games Launcher
- Visual Studio 2022 with "Game development with C++" workload + Windows SDK + .NET 6.0
- MileZero.uproject associated with UE 5.5

---

## Step 1: Generate project files and compile
```
cd <repo_root>
tools\generate_project_files.bat
tools\compile_check.bat
```
If compile fails, check docs/KNOWN_ISSUES.md for targeted fixes (KI-001 through KI-004).

## Step 2: Open the editor
```
tools\open_editor.bat
```
Or double-click `MileZero.uproject`. First launch compiles shaders — wait for it to finish.

## Step 3: Run the automated asset setup (recommended)
In the editor: **Edit > Editor Preferences > Plugins > Python > Enable Python Editor Script Plugin** (if not already enabled).

Then: **Window > Python Console** and run:
```python
exec(open("tools/setup_editor_assets.py").read())
```

This creates: all input actions, IMC_MZ_Drive mapping context, DA_MZVehicle_Hatch, DA_MZSurface_DryAsphalt, DA_MZSurface_Gravel, and L_MZ_TestTrack.

If the Python script fails or you prefer manual setup, follow steps 4-8 below.

---

## Step 4 (manual): Create Enhanced Input assets

### Input actions
In Content Browser, navigate to `Content/MileZero/Input/`, right-click > Input > Input Action:

| Name | Value Type | Notes |
|---|---|---|
| IA_MZ_Throttle | Axis1D (float) | W key, RT gamepad |
| IA_MZ_Brake | Axis1D (float) | S key, LT gamepad |
| IA_MZ_Steer | Axis1D (float) | A/D keys (A = negate modifier), LS-X gamepad |
| IA_MZ_Handbrake | Bool | Space, gamepad A |
| IA_MZ_CameraCycle | Bool | C key |
| IA_MZ_ResetVehicle | Bool | R key |
| IA_MZ_Look | Axis2D (Vector2D) | Mouse2D |

### Mapping context
Right-click > Input > Input Mapping Context > name it `IMC_MZ_Drive`.
Add mappings for each action above with the listed keys.
For IA_MZ_Steer + A key: add a **Negate** modifier.

## Step 5 (manual): Create vehicle data asset
In `Content/MileZero/Data/Vehicles/`, right-click > Miscellaneous > Data Asset > select `MZVehicleDataAsset`.
Name: `DA_MZVehicle_Hatch`. Set:
- VehicleID: `Hatch`
- DisplayName: `Practical Hatchback`
- MassKg: `1180`
- DrivetrainType: `FWD`
- GearRatios: `[3.6, 2.1, 1.4, 1.0, 0.77]`
- FinalDriveRatio: `3.9`
- MaxSteerAngle: `38`
- Engine.MaxTorque: `280`
- Engine.MaxRPM: `6800`
- Brakes.MaxBrakeTorque: `2800`
- Brakes.FrontBrakeBias: `0.65`

## Step 6 (manual): Create surface data assets
In `Content/MileZero/Data/Surfaces/`:

**DA_MZSurface_DryAsphalt**: SurfaceID=`DryAsphalt`, GripMultiplier=`1.0`, RollingResistance=`1.0`, Roughness=`0.0`

**DA_MZSurface_Gravel**: SurfaceID=`Gravel`, GripMultiplier=`0.62`, RollingResistance=`1.8`, Roughness=`0.35`

## Step 7: Create or assign a vehicle mesh
**Critical**: `AWheeledVehiclePawn` requires a skeletal mesh with wheel bones.

Easiest path: Enable **Vehicle Template** content in the editor (Add Feature/Content Pack > Vehicle), then reference its skeletal mesh in the vehicle pawn or BP wrapper.

Alternative: Create a minimal BP derived from `AMZVehiclePawn`:
1. Right-click `Content/MileZero/Blueprints/Vehicles/` > Blueprint Class > select `MZVehiclePawn` as parent > name it `BP_MZVehicle_Hatch`
2. Open BP_MZVehicle_Hatch, assign:
   - Mesh component: any vehicle skeletal mesh with 4 wheel bones
   - Wheel setups on the Chaos movement component (FL, FR, RL, RR)
   - VehicleData: `DA_MZVehicle_Hatch`
   - IA_Throttle → `IA_MZ_Throttle`, etc.
3. In `AMZGameMode` or World Settings, set DefaultPawnClass to `BP_MZVehicle_Hatch`

## Step 8: Create L_MZ_TestTrack (if not created by script)
1. File > New Level > Empty Open World or Basic
2. Save as `Content/MileZero/Maps/Test/L_MZ_TestTrack`
3. Add a large flat plane (scaled 200x200x1) for driving surface
4. Add a `PlayerStart` at `(0, 0, 100)` — this is where the vehicle spawns
5. In World Settings: set GameMode Override to `MZGameMode`

## Step 9: Wire the mapping context
In the `MZPlayerController` Blueprint defaults (or create a BP derived from it):
- Set `DrivingMappingContext` to `IMC_MZ_Drive`

Alternative: In World Settings GameMode override, set PlayerControllerClass to a BP child of `AMZPlayerController` that has `IMC_MZ_Drive` assigned.

## Step 10: PIE test
1. Open `L_MZ_TestTrack`
2. Press Alt+P (Play In Editor)
3. Verify:
   - [x] Vehicle spawns at PlayerStart
   - [x] W = throttle
   - [x] S = brake
   - [x] A/D = steer
   - [x] Space = handbrake
   - [x] C = camera cycle (chase → hood → chase)
   - [x] R = reset vehicle to spawn
4. If all pass → M1 is VERIFIED
5. Log results in docs/VERIFICATION.md and update docs/MILESTONE_STATUS.md

## Step 11: Hook verification back into docs
After each step:
- Update docs/ASSET_MANIFEST.md statuses
- Update docs/STATE.md
- Update docs/MILESTONE_STATUS.md
- Add verification entries in docs/VERIFICATION.md
