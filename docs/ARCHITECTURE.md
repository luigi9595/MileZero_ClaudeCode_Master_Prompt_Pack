# ARCHITECTURE

## Source Tree (105 files, 13,315 lines)

```
Source/MileZero/
├── MileZero.h/.cpp/.Build.cs          — Module definition, log category
│
├── Core/                               — Game framework
│   ├── MZGameMode.h/.cpp              — Spawn character + vehicle, auto-enter
│   ├── MZPlayerController.h/.cpp      — Enter/exit vehicle, input context switching, pause
│   └── MZGameInstance.h/.cpp          — Persistent state, SelectedVehicleID
│
├── Characters/                         — On-foot player
│   └── MZCharacter.h/.cpp             — Third-person character, vehicle detection sphere
│
├── Vehicles/                           — Vehicle system
│   ├── MZVehiclePawn.h/.cpp           — CORE: Chaos vehicle, input, cameras, damage/surface wiring
│   ├── MZVehicleDataAsset.h           — Data asset: mass, engine, gears, suspension
│   ├── MZVehicleRegistry.h/.cpp       — Subsystem: 3 vehicle profiles at runtime
│   ├── MZWheelFront.h/.cpp            — Front wheel defaults
│   └── MZWheelRear.h/.cpp             — Rear wheel defaults
│
├── Surfaces/                           — Surface system
│   ├── MZSurfaceDataAsset.h           — Grip, rolling resistance, roughness
│   └── MZSurfaceContactComponent.h/.cpp — Line trace surface detection
│
├── Damage/                             — Mechanical damage
│   ├── MZDamageTypes.h                — EMZImpactSeverity, FMZDamageState
│   ├── MZDamageDataAsset.h            — Impact thresholds, max damage
│   └── MZVehicleDamageComponent.h/.cpp — 8 damage subsystems, overheat
│
├── Activities/                         — Activity/mission system
│   ├── MZActivityDataAsset.h          — Activity definition (4 types)
│   ├── MZActivityHUDData.h            — HUD state struct
│   ├── MZActivityManager.h/.cpp       — Lifecycle state machine
│   ├── MZCheckpointActor.h/.cpp       — Checkpoint triggers
│   └── MZDeliveryPickup.h/.cpp        — Delivery start/end points
│
├── Economy/                            — Money system
│   ├── MZEconomyTypes.h               — FMZTransaction struct
│   └── MZEconomySubsystem.h/.cpp     — Rewards, fines, transaction history
│
├── Stats/                              — Player progression
│   ├── MZPlayerStatsSubsystem.h/.cpp  — Distance, playtime, skill, crashes
│   └── MZStatsWidget.h/.cpp           — Stats display (Slate)
│
├── Garage/                             — Repair & customization
│   ├── MZGarageActor.h/.cpp           — World-placed garage zone
│   └── MZGarageWidget.h/.cpp          — Repair/paint UI (Slate)
│
├── Save/                               — Persistence
│   ├── MZSaveGame.h/.cpp              — All save data
│   └── MZSaveManager.h/.cpp           — Save/load/autosave
│
├── Traffic/                            — AI traffic
│   ├── MZTrafficPath.h/.cpp           — Spline routes
│   ├── MZTrafficVehicle.h/.cpp        — AI vehicle (path following)
│   └── MZTrafficSubsystem.h/.cpp      — Spawn/despawn management
│
├── Pedestrians/                        — NPC pedestrians
│   ├── MZPedestrianTypes.h            — EMZPedestrianState
│   ├── MZPedestrianActor.h/.cpp       — Lightweight walker
│   └── MZPedestrianSubsystem.h/.cpp   — Zone-density spawner
│
├── World/                              — World structure
│   ├── MZWorldZoneTypes.h             — EMZZoneType
│   ├── MZWorldZone.h/.cpp             — Zone actor with bounds
│   ├── MZSpawnPoint.h/.cpp            — Spawn/recovery points
│   ├── MZWorldManager.h/.cpp          — Zone tracking subsystem
│   ├── MZRouteMarker.h/.cpp           — Route waypoints
│   ├── MZTimeOfDayManager.h/.cpp      — Day/night cycle
│   ├── MZWeatherTypes.h               — EMZWeatherState
│   └── MZWeatherManager.h/.cpp        — Wet/dry transitions
│
├── Audio/                              — Radio/sound
│   ├── MZRadioTypes.h                 — FMZRadioStation
│   ├── MZRadioManager.h/.cpp          — Station cycling
│   └── MZRadioWidget.h/.cpp           — Station change notification
│
├── Camera/                             — Photo mode
│   ├── MZPhotoMode.h/.cpp             — Free-fly + effects
│   └── MZPhotoModeWidget.h/.cpp       — Effect sliders UI
│
├── UI/                                 — HUD & menus
│   ├── MZHUD.h/.cpp                   — HUD lifecycle manager
│   ├── MZHUDWidget.h/.cpp             — Gameplay HUD (speed, RPM, gear)
│   ├── MZTelemetryWidget.h/.cpp       — Debug overlay
│   ├── MZMinimapWidget.h/.cpp         — Circular radar minimap
│   ├── MZNotificationManager.h/.cpp   — Notification subsystem
│   ├── MZNotificationWidget.h/.cpp    — Notification display
│   ├── MZPauseManager.h/.cpp          — Pause state management
│   └── MZPauseMenuWidget.h/.cpp       — Pause menu + settings
│
├── Debug/                              — Developer tools
│   ├── MZDebugSubsystem.h/.cpp        — Console commands (MZ.*)
│   └── UI/MZVehicleSelectionWidget.h/.cpp — Dev vehicle picker
│
└── Setup/                              — Auto-generation
    └── MZSetupCommandlet.h/.cpp       — Test level + main world commandlet
```

## Subsystems (auto-initialized)

| Subsystem | Scope | Purpose |
|-----------|-------|---------|
| UMZVehicleRegistry | GameInstance | Vehicle profiles (3 vehicles) |
| UMZActivityManager | GameInstance | Activity lifecycle |
| UMZSaveManager | GameInstance | Save/load/autosave |
| UMZEconomySubsystem | GameInstance | Money, rewards, fines |
| UMZPlayerStatsSubsystem | GameInstance | Stats, driving skill |
| UMZDebugSubsystem | GameInstance | Console commands |
| UMZNotificationManager | GameInstance | On-screen notifications |
| UMZPauseManager | GameInstance | Pause state |
| UMZRadioManager | GameInstance | Radio stations |
| UMZPhotoMode | GameInstance | Photo mode |
| UMZTrafficSubsystem | World | Traffic AI |
| UMZWorldManager | World | Zone tracking |
| UMZPedestrianSubsystem | World | Pedestrian spawning |

## Key Flows

### Game Start
```
GameMode::SpawnDefaultPawnAtTransform
  → Spawn AMZCharacter
  → Spawn AMZVehiclePawn (500u ahead)
  → ApplyVehicleData from Registry
  → Auto-enter vehicle (next tick)
```

### Enter/Exit Vehicle
```
F key (on foot near vehicle):
  PlayerController::RequestEnterVehicle()
    → Hide character, Possess vehicle, SwitchToDrivingInput()

F key (driving, speed < 5 km/h):
  PlayerController::RequestExitVehicle()
    → Calculate exit transform, Show character, Possess character, SwitchToOnFootInput()
```

### Vehicle Tick (per frame)
```
Surface grip (from SurfaceContactComponent)
  × Damage per-wheel multiplier
  × Weather wetness factor
  → SetWheelFrictionMultiplier(i, final)

Damage effects:
  → Throttle scaled by engine damage + overheat
  → Brake scaled by brake damage
  → Steering offset by alignment damage
```

### Economy
```
Activity complete → EconomySubsystem.AddMoney($500-1000)
Beat best time → 2x bonus
Traffic collision → -$50 fine
Garage repair → damage% × $50
Paint job → $200 flat
```
