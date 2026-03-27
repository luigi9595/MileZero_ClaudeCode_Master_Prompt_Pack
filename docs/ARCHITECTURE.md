# ARCHITECTURE

## Source Tree (72 files)

```
Source/MileZero/
├── MileZero.h/.cpp/.Build.cs     — Module definition, log category
├── Core/
│   ├── MZGameMode.h/.cpp         — Game mode, vehicle spawn via registry
│   ├── MZPlayerController.h/.cpp — Input context management
│   └── MZGameInstance.h/.cpp     — Persistent state, SelectedVehicleID
├── Characters/
│   └── MZCharacter.h/.cpp        — On-foot character (stubbed for post-v1)
├── Vehicles/
│   ├── MZVehiclePawn.h/.cpp      — CORE: Chaos vehicle, input, camera, surface+damage wiring
│   ├── MZVehicleDataAsset.h      — Data asset: mass, engine, gears, suspension, steering
│   ├── MZVehicleRegistry.h/.cpp  — Subsystem: creates 3 vehicle profiles at runtime
│   ├── MZWheelFront.h/.cpp       — Front wheel defaults
│   └── MZWheelRear.h/.cpp        — Rear wheel defaults
├── Surfaces/
│   ├── MZSurfaceDataAsset.h      — Grip, rolling resistance, roughness
│   └── MZSurfaceContactComponent.h/.cpp — Line trace surface detection
├── Damage/
│   ├── MZDamageTypes.h           — EMZImpactSeverity, FMZDamageState
│   ├── MZDamageDataAsset.h       — Impact thresholds, max damage values
│   └── MZVehicleDamageComponent.h/.cpp — 8 damage subsystems, overheat, per-wheel
├── UI/
│   ├── MZHUD.h/.cpp              — AHUD lifecycle manager
│   ├── MZHUDWidget.h/.cpp        — Gameplay HUD (Slate): speed, RPM, gear, damage
│   └── MZTelemetryWidget.h/.cpp  — Debug overlay (Slate): full telemetry, FPS, grip color
├── Activities/
│   ├── MZActivityDataAsset.h     — Activity definition (4 types)
│   ├── MZActivityHUDData.h       — HUD data struct for activity state
│   ├── MZActivityManager.h/.cpp  — Subsystem: activity lifecycle state machine
│   ├── MZCheckpointActor.h/.cpp  — Trigger volumes for checkpoint sprint
│   └── MZDeliveryPickup.h/.cpp   — Pickup/dropoff points for delivery run
├── Save/
│   ├── MZSaveGame.h/.cpp         — Save data: vehicle, transform, activities, settings
│   └── MZSaveManager.h/.cpp      — Subsystem: save/load/autosave
├── Debug/
│   ├── MZDebugSubsystem.h/.cpp   — Console commands: MZ.*, vehicle spawn, telemetry toggle
│   └── UI/MZVehicleSelectionWidget.h/.cpp — Dev vehicle picker (Slate)
├── Setup/
│   └── MZSetupCommandlet.h/.cpp  — Auto-generates test level + main world
├── Traffic/
│   ├── MZTrafficPath.h/.cpp      — Spline-based traffic routes
│   ├── MZTrafficVehicle.h/.cpp   — Simple AI vehicle (APawn + path following)
│   └── MZTrafficSubsystem.h/.cpp — World subsystem: spawn/despawn loop
└── World/
    ├── MZWorldZoneTypes.h        — EMZZoneType enum
    ├── MZWorldZone.h/.cpp        — Zone actor with bounds + overlap detection
    ├── MZSpawnPoint.h/.cpp       — Spawn/recovery point actors
    ├── MZWorldManager.h/.cpp     — World subsystem: zone tracking, spawn queries
    ├── MZRouteMarker.h/.cpp      — Route waypoint actors
    ├── MZTimeOfDayManager.h/.cpp — Sun rotation, intensity, color blending
    ├── MZWeatherTypes.h          — EMZWeatherState enum
    └── MZWeatherManager.h/.cpp   — Wetness factor, grip modifier
```

## Key Data Flow

### Vehicle Tick (every frame)
```
MZVehiclePawn::Tick()
  → SurfaceContactComponent.GetCurrentGripMultiplier()     // base grip from surface
  → DamageComponent.GetWheelGripMultiplier(i)               // per-wheel damage penalty
  → SetWheelFrictionMultiplier(i, baseGrip * damageGrip)   // applied to Chaos wheels
  → ApplyDamageEffects()
      → throttle scaled by PowerMultiplier + OverheatFactor
      → brake scaled by BrakeMultiplier
      → steering offset by SteeringPullDegrees
```

### Collision → Damage Flow
```
OnActorHit → DamageComponent.ProcessImpact(speedKmh, normal, point)
  → CalculateSeverity (Light/Medium/Heavy from speed thresholds)
  → ApplyDamageToSubsystems (direction-aware distribution)
      Front hit → engine, cooling, front suspension, aero
      Side hit  → steering, near-side wheels/suspension
      Rear hit  → rear suspension, aero
```

### Vehicle Spawn Flow
```
GameMode.SpawnDefaultPawnAtTransform()
  → GameInstance.SelectedVehicleID
  → VehicleRegistry.GetVehicleData(ID)
  → SpawnActor<AMZVehiclePawn>()
  → Vehicle.ApplyVehicleData(data)  // mass, engine, gears, steering, drag
  → Controller.Possess(vehicle)
```

### Activity Flow
```
ActivityManager.StartActivity(data)
  → state = Starting → Active
  → CheckpointActor.OnOverlapBegin → ActivityManager.OnCheckpointReached()
  → Timer tick → check time limit
  → CompleteActivity() / FailActivity()
  → SaveManager.AutoSave()  // persist best times
```

## Subsystems (auto-initialized)
| Subsystem | Scope | Purpose |
|-----------|-------|---------|
| UMZVehicleRegistry | GameInstance | Vehicle data management |
| UMZActivityManager | GameInstance | Activity lifecycle |
| UMZSaveManager | GameInstance | Save/load/autosave |
| UMZDebugSubsystem | GameInstance | Console commands, dev tools |
| UMZTrafficSubsystem | World | Traffic AI management |
| UMZWorldManager | World | Zone tracking, spawn queries |
