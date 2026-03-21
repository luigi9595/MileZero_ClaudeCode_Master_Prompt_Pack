# ARCHITECTURE

## High-level modules
- Core — game mode, player controller, game instance
- Data — UPrimaryDataAsset subclasses for vehicles, surfaces, activities
- Input — Enhanced Input contexts and actions (editor assets)
- Characters — on-foot player character (AMZCharacter)
- Vehicles — vehicle pawn, vehicle data, damage component
- Traffic — traffic subsystem, traffic vehicles (M7)
- Activities — activity manager, activity data assets (M6)
- World — world partition, data layers (M5)
- UI — HUD, telemetry, pause menu, main menu widgets (M6)
- Debug — debug subsystem, console commands, telemetry toggle
- Save — save game class, persistence (M6)

## Architectural principles
- C++ first for core gameplay systems
- Blueprints only where composition speed matters and logic remains understandable
- Data-driven tuning for vehicles, surfaces, damage, activities, and traffic
- Milestone-based implementation with verification gates
- Avoid magic numbers and scattered tuning

## Source tree (current)
```
Source/
  MileZero.Target.cs
  MileZeroEditor.Target.cs
  MileZero/
    MileZero.Build.cs
    MileZero.h / .cpp                       — module definition + log category
    Core/
      MZGameMode.h / .cpp                   — game mode, default pawn spawn
      MZPlayerController.h / .cpp           — input context switching, vehicle enter/exit stubs
      MZGameInstance.h / .cpp               — persistent game state, vehicle selection
    Characters/
      MZCharacter.h / .cpp                  — third-person on-foot character
    Vehicles/
      MZVehiclePawn.h / .cpp                — Chaos wheeled vehicle, input, camera, reset, telemetry
      MZVehicleDataAsset.h                  — UPrimaryDataAsset for vehicle specs
    Surfaces/
      MZSurfaceDataAsset.h                  — UPrimaryDataAsset for surface profiles
      MZSurfaceContactComponent.h / .cpp    — runtime surface detection
    Debug/
      MZDebugSubsystem.h / .cpp             — console commands, telemetry toggle
    Save/
      MZSaveGame.h / .cpp                   — USaveGame subclass
    Activities/
      MZActivityDataAsset.h                 — UPrimaryDataAsset for activity definitions
```

## Content tree (planned)
```
Content/MileZero/
  Maps/Boot/         — L_MZ_Boot
  Maps/Frontend/     — L_MZ_Frontend
  Maps/Test/         — L_MZ_TestTrack
  Maps/World/        — L_MZ_MainWorld
  Input/             — IMC_MZ_OnFoot, IMC_MZ_Drive, IMC_MZ_UI, IA_MZ_*
  Data/Vehicles/     — DA_MZVehicle_Coupe, _Hatch, _Pickup
  Data/Surfaces/     — DA_MZSurface_DryAsphalt, _WetAsphalt, _Gravel, _Grass
  Data/Activities/   — DA_MZActivity_*
  UI/                — WBP_MZHUD, WBP_MZPauseMenu, WBP_MZMainMenu, WBP_MZTelemetry
  Meshes/Vehicles/   — skeletal meshes
  Meshes/World/      — world geometry
  Materials/         — surface materials, vehicle materials
  Blueprints/Vehicles/ — BP wrappers if needed
```

## Key class relationships
- AMZGameMode sets DefaultPawnClass to AMZVehiclePawn (M1 fallback)
- AMZPlayerController manages Enhanced Input context switching
- AMZVehiclePawn owns USpringArmComponent + UCameraComponent (chase/hood)
- AMZVehiclePawn reads UMZVehicleDataAsset for tuning
- UMZSurfaceContactComponent traces ground and resolves UMZSurfaceDataAsset
- UMZDebugSubsystem provides exec console commands
- UMZSaveGame stores player state
