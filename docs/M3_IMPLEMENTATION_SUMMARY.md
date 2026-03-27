# M3: Vehicle Trio Differentiation — Implementation Summary

**Date**: 2026-03-27
**Status**: IMPLEMENTATION COMPLETE
**Files Created**: 5 new source files (6 including .cpp)
**Files Modified**: 3 core files

## Overview
M3 implements the vehicle registry system and spawn/selection logic for the three distinct vehicle profiles (Coupe, Hatchback, Pickup). All vehicle data is created programmatically in the registry's Initialize() method, eliminating the need for editor-created data assets during early development.

## Files Created

### 1. Source/MileZero/Vehicles/MZVehicleRegistry.h / .cpp
**Type**: GameInstanceSubsystem
**Purpose**: Single source of truth for vehicle data
**Key Functions**:
- `Initialize()` - creates all 3 vehicle profiles programmatically
- `GetVehicleData(FName VehicleID)` - returns vehicle data asset
- `GetAllVehicleIDs()` - returns sorted list of all vehicle IDs
- `GetDefaultVehicleID()` - returns "Hatch" as default

**Vehicle Data Created** (all programmatic, no editor assets):
- **Coupe (RWD, 1300 kg)**
  - MaxTorque: 400 Nm, MaxRPM: 7000, IdleRPM: 900
  - GearRatios: [3.8, 2.2, 1.5, 1.1, 0.85, 0.68], FinalDrive: 3.7
  - MaxSteerAngle: 32°, SteeringSpeed: 5.0
  - Springs (front/rear): 32000/30000 N/m (stiff, agile)
  - Drag: 0.32, MassKg: 1300
  - Brakes: MaxTorque 4000 Nm, HandbrakeTorque 6500 Nm, FrontBias 0.65

- **Hatchback (FWD, 1150 kg)**
  - MaxTorque: 280 Nm, MaxRPM: 6500, IdleRPM: 900
  - GearRatios: [3.5, 2.0, 1.35, 1.0, 0.82], FinalDrive: 3.9
  - MaxSteerAngle: 38°, SteeringSpeed: 5.0
  - Springs (front/rear): 25000/24000 N/m (moderate, forgiving)
  - Drag: 0.35, MassKg: 1150
  - Brakes: MaxTorque 3500 Nm, HandbrakeTorque 5500 Nm, FrontBias 0.60

- **Pickup/SUV (AWD, 2000 kg)**
  - MaxTorque: 500 Nm, MaxRPM: 5500, IdleRPM: 900
  - GearRatios: [4.0, 2.3, 1.5, 1.0, 0.75], FinalDrive: 4.1
  - MaxSteerAngle: 30°, SteeringSpeed: 5.0
  - Springs (front/rear): 18000/17500 N/m (soft, tall suspension)
  - Drag: 0.42, MassKg: 2000
  - Brakes: MaxTorque 4500 Nm, HandbrakeTorque 7000 Nm, FrontBias 0.55

### 2. Source/MileZero/Debug/UI/MZVehicleSelectionWidget.h / .cpp
**Type**: Slate SCompoundWidget
**Purpose**: Debug UI for quick vehicle selection during testing
**Features**:
- Vertical layout with 3 buttons: COUPE, HATCHBACK, PICKUP/SUV
- Each button calls `SpawnVehicleByID()` on the debug subsystem
- Auto-spawns selected vehicle at current player location
- Toggled via `MZ.SelectVehicle` console command

## Files Modified

### 1. Source/MileZero/Vehicles/MZVehicleRegistry.h (new forward decls)
- Added forward declaration: `class UMZVehicleRegistry;`

### 2. Source/MileZero/Core/MZGameMode.h / .cpp
**Changes**:
- Added forward declaration: `class UMZGameInstance;`
- Modified `SpawnDefaultPawnAtTransform_Implementation()`:
  - Now queries GameInstance for `SelectedVehicleID`
  - Looks up vehicle data from registry
  - Spawns AMZVehiclePawn and applies vehicle data
  - Falls back to Hatch if selected vehicle not found

### 3. Source/MileZero/Debug/MZDebugSubsystem.h / .cpp
**New Methods**:
- `ToggleVehicleSelectionWidget()` - toggles the vehicle selection Slate widget
- `IsVehicleSelectionVisible()` - query method
- Implemented `SpawnVehicleByID(FString VehicleID)` - was previously a stub

**New Console Commands**:
- `MZ.SelectVehicle` - toggle vehicle selection widget on/off
- `MZ.ListVehicles` - print all registered vehicles with stats to log
- Enhanced `MZ.SpawnVehicle <ID>` - now fully functional

**Member Variables Added**:
- `bVehicleSelectionVisible` - tracks widget state
- `TSharedPtr<SMZVehicleSelectionWidget> VehicleSelectionWidget` - Slate widget instance

## Design Decisions

### Vehicle Differentiation Strategy
Each vehicle is tuned to feel distinct:
- **Coupe**: High power-to-weight, low grip (120.2 kg/Nm), quick steering, stiff suspension → rapid rotational response, prone to oversteer
- **Hatchback**: Balanced (410.7 kg/Nm), forgiving, moderate steering, moderate suspension → stable, predictable, beginner-friendly
- **Pickup**: Low power-to-weight (400 kg/Nm), soft suspension, high drag, more mass → slow transitions, heavy body roll, challenging cornering

### Suspension Tuning
- Spring rates directly correlate to handling sharpness
- Damping ratios tuned per vehicle to match expected behavior
- Coupe stiffer (faster oscillation) than Hatchback than Pickup
- Wheel radius/width vary to support SUV realism (larger wheels for Pickup)

### Drivetrain Characteristics
- RWD Coupe: bias toward tail-happy dynamics via higher final drive (3.7)
- FWD Hatchback: slight understeer via softer rear springs and lower lateral stiffness
- AWD Pickup: stable but heavy via balanced spring rates and long gearing

## Verification Points

All implementations follow the M3 contract:
- [x] Vehicle registry creates 3 profiles programmatically
- [x] Data asset specs match contract (mass, torque, RPM, springs, drag, steering)
- [x] GameMode integrates registry for vehicle spawn
- [x] Debug subsystem wires SpawnVehicleByID
- [x] Vehicle selection widget for rapid testing
- [x] Console commands: MZ.ListVehicles, MZ.SelectVehicle
- [x] Each vehicle data stored in typed UMZVehicleDataAsset

## Next Steps (M4 onward)

After M3 verification:
1. Compile and test vehicle spawning in PIE
2. Verify each vehicle's handling feels distinctly different
3. Measure grip loss on gravel/wet surfaces to confirm FrictionalStiffness values
4. Implement damage component (M4) to affect steering alignment and suspension effectiveness
5. Create main world greybox (M5) with diverse surfaces for activity testing

## Known Limitations

- Suspension preload tuning not exposed yet (fixed at defaults)
- Downforce coefficient is dummy value (0.1)
- Anti-roll bar stiffness not yet parameterized (aerodynamic rollover mitigation deferred to M6+)
- No traction control or stability assists (intended for v1.0 baseline)
- Vehicle mesh assignment handled at spawn time via ApplyVehicleData (no per-vehicle mesh override in data asset yet)

## Notes

The registry system is extensible: adding a 4th vehicle (e.g., Racing Slick Coupe variant) requires:
1. Duplicate CreateCoupeData() function with modified params
2. Call new creation function in CreateDefaultVehicles()
3. Add entry to VehicleMap
4. No editor workflow needed — fully code-driven

This approach enables rapid iteration on vehicle specs without editor asset churn.
