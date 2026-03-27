# M6 Implementation: Activity Manager and Enhanced Save/Load

## Files Created

### 1. Activities Module

#### MZActivityHUDData.h
- Simple struct for passing activity state to HUD widgets
- Fields: ObjectiveText, ElapsedTime, TimeLimit, CurrentCheckpoint, TotalCheckpoints, DamagePercent, ActivityType, bIsActive
- Blueprint-compatible for widget binding

#### MZCheckpointActor.h/.cpp
- Actor with UBoxComponent TriggerVolume for overlap detection
- Optional UStaticMeshComponent VisualMesh for visualization
- Properties: CheckpointIndex, bIsStart, bIsFinish
- States: bIsReachable, bIsReached
- OnCheckpointReached delegate broadcasts index and finish flag
- Visual colors: Green (active), Grey (passed), Red (unreachable)
- SetCheckpointState() manages reachability and visual updates

#### MZDeliveryPickup.h/.cpp
- Actor with UBoxComponent PickupZone for delivery points
- Optional UStaticMeshComponent VisualMesh
- Properties: DeliveryID, bIsPickup, PickupRadius
- OnDeliveryInteraction delegate broadcasts DeliveryID and pickup flag
- Visual colors: Blue (pickup), Yellow (dropoff)
- SetDeliveryActive() controls zone collision and visual state

### 2. Activity Manager

#### MZActivityManager.h/.cpp
- UGameInstanceSubsystem managing activity lifecycle
- State enum: Inactive, Starting, Active, Completed, Failed

**State Management:**
- CurrentActivity: current data asset
- ActivityState: current state enum
- ElapsedTime: seconds elapsed
- CurrentCheckpointIndex: 0-based index or -1
- AccumulatedDamage: sum of damage types
- ActiveCheckpoints: array of checkpoint actors
- ActiveDeliveryPoints: array of delivery points
- BestTimes: FName → float map for best times
- bCargoPickedUp: delivery run state
- TrafficVehiclesPassed: weave activity counter

**Core Methods:**
- StartActivity(Activity): setup and transition to Active
- AbandonActivity(): fail with "abandoned" reason
- CompleteActivity(): record best time, broadcast success
- FailActivity(Reason): mark failed and cleanup
- TickActivity(DeltaTime): update timers and check conditions

**Checkpoint Handling:**
- OnCheckpointReached(Index, bIsFinish): updates state, unlocks next
- SetupCheckpointSprint(): finds all checkpoints, sorts by index

**Delivery Handling:**
- OnDeliveryPickedUp(ID): marks cargo state, enables dropoffs
- OnDeliveryDroppedOff(ID): completes activity
- SetupDeliveryRun(): finds all delivery points

**Traffic Weave:**
- IncrementTrafficVehiclesPassed(): counter for pass tracking
- SetupTrafficWeave(): initializes counter

**Damage Survival:**
- UpdateAccumulatedDamage(Amount): tracks damage, fails if exceeds limit
- SetDamageFromState(FMZDamageState): sums all damage types
- SetupDamageSurvival(): initializes tracking

**Delegates:**
- OnActivityStateChanged: broadcasts new state, message, success flag
- OnActivityProgressUpdated: broadcasts FMZActivityHUDData

**Activity-Specific Logic:**
- CheckpointSprint: sequential checkpoint progression, finish trigger completes
- DeliveryRun: pickup unlocks dropoff points, dropoff completes
- TrafficWeave: manual pass counting, time limit runs
- DamageSurvival: damage accumulation with limit check, time limit runs

### 3. Save/Load System

#### MZSaveGame.h (Enhanced)
Added:
- BestTimes: TMap<FName, float> for activity records
- TotalPlayTimeSeconds: cumulative playtime
- CurrentMapName: save location
- DamageState: FMZDamageState struct
- PlayerSettings: FMZPlayerSettings struct
  - MusicVolume, SFXVolume, bShowTelemetry

#### MZSaveManager.h/.cpp
- UGameInstanceSubsystem for save/load management
- CurrentSaveGame: loaded/working save object
- PlaytimeAccumulator: tracks time since last autosave
- AutosaveIntervalSeconds: 60 second interval

**Core Methods:**
- SaveGame(SlotName): capture state, save to disk, broadcast result
- LoadGame(SlotName): load from disk, apply state, broadcast result
- AutoSave(): save to default slot
- HasSaveData(SlotName): check if save exists
- DeleteSave(SlotName): delete save slot
- GetCurrentSaveGame(): return loaded object

**State Management:**
- CaptureGameState(SaveGame): capture current world state
  - CurrentMapName from GetMapName()
  - PlayerTransform from first player pawn
  - Playtime accumulated in TotalPlayTimeSeconds
- ApplyGameState(SaveGame): apply saved state to world
  - Restore player pawn transform
  - Load dependent on map context

**Autosave System:**
- UpdatePlaytime(DeltaTime): accumulates time, triggers autosave at interval
- Called from game mode tick or character tick
- Resets accumulator after save

**Slot Management:**
- GetActiveSlotName(): use DefaultSlotName if empty
- UGameplayStatics integration for disk I/O

**Delegates:**
- OnSaveStateChanged: broadcasts success flag and message

## Architecture Notes

### Separation of Concerns
- Activity Manager handles gameplay progression logic
- Save Manager handles serialization and persistence
- Checkpoint/Delivery actors handle trigger detection only
- HUD data struct decouples display from state

### Thread Safety
- All operations use main thread (game instance subsystems)
- No async or parallel activity code in M6

### Extensibility
- Activity types are enum-based (add to EMZActivityType)
- New activity types require:
  1. Setup method (SetupXxx)
  2. Condition check in CheckActivityCompletion/CheckActivityFailure
  3. Tick handling if needed
  4. UI update logic (GetCurrentObjectiveText)

### Save/Load Integration
- SaveGame captures platform-agnostic state
- ApplyGameState designed for world context awareness
- AutoSave prevents player loss with 60-second intervals
- Delegates allow UI feedback on save/load operations

## Verification Checklist

- [ ] All .h files have #pragma once and GENERATED_BODY()
- [ ] All .cpp files #include "MileZero.h"
- [ ] Activity delegates properly bound/unbound in Setup/Clear
- [ ] Checkpoint reachability updates correctly
- [ ] Delivery dropoffs disabled until pickup
- [ ] Time limits trigger failure in TickActivity
- [ ] Best times stored in map and persist across saves
- [ ] Autosave accumulator resets after save
- [ ] Player transform captured and restored correctly

## Integration Points

### Required from Game Mode:
- Call SaveManager->UpdatePlaytime(DeltaTime) in game tick
- Provide world context for actor iteration (checkpoints/deliveries)

### Required from Vehicle System:
- Call ActivityManager->UpdateAccumulatedDamage() on collision
- Call ActivityManager->SetDamageFromState() after damage calc
- Inform ActivityManager of traffic vehicle passes

### Required from HUD:
- Bind widgets to OnActivityProgressUpdated delegate
- Display GetHUDData() in activity widget
- Show saved activity times from GetBestTime()

### Optional Integrations:
- Link PlayerSettings (MusicVolume, bShowTelemetry) to audio/debug systems
- Use CurrentMapName for level-specific recovery logic
