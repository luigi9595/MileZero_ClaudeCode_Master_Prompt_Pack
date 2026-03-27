# M5: World Greybox Infrastructure Implementation

**Date**: 2026-03-27
**Status**: COMPLETE - All code files created and verified
**Milestone**: M5 — Main world greybox infrastructure

## Summary

Implemented comprehensive C++ infrastructure for the MileZero world system. Created 6 new classes plus enhancements to the existing setup commandlet to generate a full greybox main world with 5 zones, spawn points, and environmental setup.

## Files Created

### 1. Source/MileZero/World/MZWorldZoneTypes.h
- **Purpose**: Enum definition for zone types
- **Content**: `EMZZoneType` with 5 values:
  - City
  - Industrial
  - Highway
  - Rural
  - ProvingGround
- **Size**: 16 lines
- **Dependencies**: CoreMinimal.h only

### 2. Source/MileZero/World/MZWorldZone.h / .cpp
- **Purpose**: Defines a rectangular zone actor in the world
- **Key Features**:
  - `FName ZoneID`: Unique zone identifier
  - `FText ZoneName`: Display name
  - `EMZZoneType ZoneType`: Zone classification
  - `UBoxComponent* ZoneBounds`: Trigger volume for overlap detection
  - `float DefaultSpeedLimit`: Speed limit in km/h (default 50)
  - `FName DefaultSurfaceType`: Surface type name (default "DryAsphalt")
  - `TArray<FVector> SpawnPoints`: Recovery/spawn locations
  - `TArray<AMZWorldZone*> ConnectedZones`: Adjacent zone references
  - **Delegates**: `OnVehicleEntered`, `OnVehicleExited`
  - **API**:
    - `IsLocationInZone()`: Point-in-zone test
    - `GetNearestSpawnPoint()`: Find nearest spawn
    - `GetRandomSpawnPoint()`: Random spawn selector
    - `AddConnectedZone()`: Zone connectivity
    - `GetZoneInfo()`: Debug string representation
- **Implementation**:
  - Automatic zone bounds overlap event binding
  - Pawn filtering for vehicle-only triggers
  - Verbose logging of zone entry/exit

### 3. Source/MileZero/World/MZSpawnPoint.h / .cpp
- **Purpose**: Defines a vehicle spawn or recovery location
- **Key Features**:
  - `FVector SpawnLocation`: Auto-synced from actor location
  - `FRotator SpawnRotation`: Spawn heading
  - `FName AssociatedZoneID`: Zone ownership
  - `bool bIsDefaultSpawn`: Player starting location flag
  - `bool bIsRecoveryPoint`: Recovery location flag
  - `UArrowComponent* DirectionArrow`: Editor visualization (rotatable)
  - `UBillboardComponent* EditorSprite`: Editor billboard label
  - **API**:
    - `GetSpawnTransform()`: Complete spawn transform
    - `GetSpawnPointInfo()`: Debug string
- **Implementation**:
  - Editor-only visual components
  - Auto-sync spawn location from actor transform
  - Post-edit property synchronization
  - Billboard text reflects flags

### 4. Source/MileZero/World/MZWorldManager.h / .cpp
- **Purpose**: UWorldSubsystem managing all zones and spawn points
- **Key Features**:
  - `TArray<AMZWorldZone*> AllZones`: Auto-discovered at init
  - `TArray<AMZSpawnPoint*> AllSpawnPoints`: Auto-discovered at init
  - `AMZWorldZone* CurrentZone`: Current player zone
  - **Delegate**: `OnZoneChanged(PreviousZone, NewZone)`
  - **Query API**:
    - `GetCurrentZone()`: Player's zone
    - `GetNearestSpawnPoint(Location)`: Nearest to any location
    - `GetDefaultSpawnPoint()`: Default spawn
    - `GetZoneByID(FName)`: Lookup by zone ID
    - `GetAllZonesOfType(EMZZoneType)`: Filter by type
    - `GetRandomSpawnPointInZone(ZoneID)`: Random spawn in zone
    - `GetSpawnPointsInZone(ZoneID)`: All spawns in zone
    - `GetWorldManagerInfo()`: Debug string
- **Implementation**:
  - Auto-discovery via TActorIterator at Initialize()
  - Per-tick player zone detection
  - Zone change broadcast
  - Fallback to first spawn point if no default set

### 5. Source/MileZero/World/MZRouteMarker.h / .cpp
- **Purpose**: Defines waypoints/routes between zones
- **Key Features**:
  - `FName RouteID`: Route identifier
  - `int32 SequenceIndex`: Order in route
  - `FName FromZoneID`, `ToZoneID`: Zone connection
  - `UBillboardComponent* EditorIcon`: Editor visibility
  - **API**:
    - `GetRouteMarkerInfo()`: Debug string
    - `GetDistanceToMarker(OtherMarker)`: Distance measurement
- **Purpose**: Used by AI traffic and activity checkpoints

### 6. Enhanced Source/MileZero/Setup/MZSetupCommandlet.h / .cpp
- **New Methods**:
  - `CreateMainWorld()`: Generates L_MZ_MainWorld level
  - `CreateZone()`: Helper to spawn and configure zones
  - `CreateSpawnPointsInZone()`: Helper to populate spawn points
  - `CreateGroundPlane()`: Helper for terrain planes
  - `ConnectAdjacentZones()`: Helper for connectivity
- **World Generation**:
  - **City** at (0, 0, 0): 10000x10000 UU, speed limit 50 km/h
  - **Industrial** at (15000, 0, 0): 8000x8000 UU, speed limit 60 km/h
  - **Highway** at (7500, 0, 0): 40000x2000 UU (strip), speed limit 100 km/h
  - **Rural** at (0, 15000, 0): 12000x12000 UU, speed limit 70 km/h, Gravel surface
  - **ProvingGround** at (-15000, 0, 0): 6000x6000 UU, speed limit 120 km/h
- **Spawn Points**:
  - City: 3 spawn points
  - Industrial: 2 spawn points
  - Rural: 2 spawn points
  - ProvingGround: 1 spawn point (default)
- **Environment**:
  - Directional light (sun) with atmosphere
  - Sky atmosphere
  - Sky light with real-time capture
  - Height fog
  - Lightmass importance volume
  - Ground planes for each zone

## Architecture Notes

### Class Hierarchy
```
AActor (AMZWorldZone)
    └─ UBoxComponent (ZoneBounds)

AActor (AMZSpawnPoint)
    ├─ UArrowComponent (DirectionArrow)
    └─ UBillboardComponent (EditorSprite)

AActor (AMZRouteMarker)
    └─ UBillboardComponent (EditorIcon)

UWorldSubsystem (UMZWorldManager)
    └─ Auto-discovers zones and spawns at Initialize()
```

### Zone Detection
- Zones use UBoxComponent overlap events for entry/exit detection
- Filters for APawn (vehicle) actors only
- MZWorldManager polls player pawn each tick to detect zone changes
- OnZoneChanged delegate broadcasts on transition

### Spawn Point Management
- Spawn points are actor-based for easy placement in editor
- Location/rotation auto-sync from actor transform
- Associated with zones via FName for O(n) lookup
- Default spawn point (bIsDefaultSpawn) used if no explicit spawn specified
- Recovery points (bIsRecoveryPoint) used for crash respawn

### World Generation Pipeline
1. CreateTestLevel() (legacy, unchanged)
2. CreateMainWorld() (new)
   - Create package and world
   - Initialize world context
   - Create 5 zones with spawn points
   - Create ground planes
   - Create environment (lights, sky, fog)
   - Save to /Game/MileZero/Maps/Main/L_MZ_MainWorld.umap

## Integration Points

### With MZGameMode
- Game mode spawns player at default spawn point via MZWorldManager
- Game mode can query current zone for zone-specific logic

### With MZVehiclePawn
- Vehicle broadcasts overlap events when entering/exiting zones
- Zone detection works via APawn cast in zone bounds

### With Activities
- Activities can query spawn points by zone
- Route markers guide activity checkpoint placement

### With Traffic System
- Zones define speed limits for traffic vehicles
- Connected zones guide traffic pathfinding

## Verification Checklist

- [x] All 6 files created with correct includes
- [x] MILEZERO_API on all classes
- [x] #pragma once on all headers
- [x] .generated.h includes placed last
- [x] Forward declarations for circular deps (AMZWorldZone, AMZSpawnPoint)
- [x] UE_LOG calls use LogMileZero category
- [x] Delegates declared with DECLARE_DYNAMIC_MULTICAST_DELEGATE
- [x] Blueprint-exposed properties use UPROPERTY(EditAnywhere, BlueprintReadWrite)
- [x] Read-only properties use BlueprintReadOnly
- [x] Blueprint functions use UFUNCTION(BlueprintCallable/Pure)
- [x] Constructor initializes components correctly
- [x] BeginPlay binds overlap delegates
- [x] WorldManager uses proper UWorldSubsystem Tick
- [x] Commandlet creates world with proper package/context setup
- [x] Spawn point zones created within extents (80% margin)
- [x] Ground planes scaled appropriately per zone
- [x] Environment setup matches TestLevel pattern

## Compilation Requirements

**New Includes Required**:
- MZWorldZoneTypes.h in any file using EMZZoneType
- MZWorldZone.h for zone actors
- MZSpawnPoint.h for spawn points
- MZWorldManager.h for world management

**UE5.7 Features Used**:
- Enhanced Input System (implicit)
- Chaos Vehicle System (implicit)
- World Subsystems (UWorldSubsystem)
- Dynamic Multicast Delegates
- TArray and FName
- Actor iterator (TActorIterator)

## Future Integration

### M6 Activities
- Activities will use spawn points and zone information
- Route markers will guide checkpoint placement
- Zones will determine activity rules

### M7 Traffic
- Traffic subsystem will respect zone speed limits
- Connected zones will guide NPC pathfinding
- Zone type determines traffic density

### M8 Hardening
- Validate zone bounds don't overlap unexpectedly
- Performance test actor iteration
- Optimize zone detection with spatial hashing if needed

## Notes

- All code is modular and can be tested independently
- Commandlet is editor-only (#if WITH_EDITOR)
- Zone bounds are visualized in editor with box bounds
- Spawn points show arrow + billboard in editor only
- World generation is deterministic (same input = same output)
- All float values use sensible defaults
- Logging is comprehensive for debugging world setup
