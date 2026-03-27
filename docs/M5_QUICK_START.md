# M5 Quick Start Guide

## Overview
M5 implements the world greybox infrastructure: zones, spawn points, and world management for MileZero.

## Files Created

**Zone System**
- `MZWorldZoneTypes.h` - Zone type enum
- `MZWorldZone.h/.cpp` - Zone actor with bounds and detection

**Spawn System**
- `MZSpawnPoint.h/.cpp` - Spawn point actor with zone association

**World Management**
- `MZWorldManager.h/.cpp` - UWorldSubsystem for global world queries

**Route System**
- `MZRouteMarker.h/.cpp` - Route waypoint for pathfinding

**Enhanced**
- `MZSetupCommandlet.cpp` - Added CreateMainWorld() to generate L_MZ_MainWorld

## How to Use

### Running the Commandlet
```bash
UnrealEditor-Cmd MileZero.uproject -run=MZSetup
```

This creates:
- L_MZ_MainWorld with 5 zones
- 8 spawn points distributed across zones
- Full environment (lights, sky, fog)

### Accessing World Manager
```cpp
// From a game mode or other system:
UWorld* World = GetWorld();
AMZWorldManager* Manager = World->GetSubsystem<AMZWorldManager>();

// Get current zone
AMZWorldZone* CurrentZone = Manager->GetCurrentZone();

// Get spawn point
AMZSpawnPoint* Spawn = Manager->GetDefaultSpawnPoint();

// Query zones
TArray<AMZWorldZone*> CityZones;
Manager->GetAllZonesOfType(EMZZoneType::City, CityZones);
```

### Zone Properties in Editor
```
Zone ID - Unique identifier (e.g., "City", "Industrial")
Zone Name - Display name
Zone Type - Dropdown (City, Industrial, Highway, Rural, ProvingGround)
Speed Limit - km/h (converted to UU/s at runtime)
Surface Type - Name (e.g., "DryAsphalt", "Gravel")
Spawn Points - Array of FVector locations
Connected Zones - Array of AMZWorldZone* references
```

### Spawn Point Properties in Editor
```
Zone ID - Associated zone identifier
Is Default Spawn - True for player starting location
Is Recovery Point - True for crash respawn locations
Rotation - Heading direction
```

## World Layout (L_MZ_MainWorld)

```
                    RURAL (0, 15000, 0)
                    12k x 12k
                         |
PROVING GROUND (-15k, 0, 0)    CITY (0, 0, 0)    INDUSTRIAL (15k, 0, 0)
6k x 6k                        10k x 10k          8k x 8k
      |__________________________|__________________|
                    HIGHWAY (strip)
                    40k x 2k, 100 km/h
```

## Key APIs

### AMZWorldZone
```cpp
bool IsLocationInZone(const FVector& Location) const;
FVector GetNearestSpawnPoint(const FVector& Location) const;
FVector GetRandomSpawnPoint() const;
void AddConnectedZone(AMZWorldZone* OtherZone);
```

### AMZSpawnPoint
```cpp
FTransform GetSpawnTransform() const;
FString GetSpawnPointInfo() const;
```

### UMZWorldManager
```cpp
AMZWorldZone* GetCurrentZone() const;
AMZSpawnPoint* GetNearestSpawnPoint(const FVector& Location) const;
AMZSpawnPoint* GetDefaultSpawnPoint() const;
AMZWorldZone* GetZoneByID(const FName& ZoneID) const;
void GetAllZonesOfType(EMZZoneType ZoneType, TArray<AMZWorldZone*>& OutZones) const;
AMZSpawnPoint* GetRandomSpawnPointInZone(const FName& ZoneID) const;
void GetSpawnPointsInZone(const FName& ZoneID, TArray<AMZSpawnPoint*>& OutSpawnPoints) const;
```

### Delegates
```cpp
// Zone changed
OnZoneChanged.AddDynamic(this, &AMyClass::OnZoneChangedCallback);

// Vehicle enter/exit zone
Zone->OnVehicleEntered.AddDynamic(this, &AMyClass::OnVehicleEnteredZone);
Zone->OnVehicleExited.AddDynamic(this, &AMyClass::OnVehicleExitedZone);
```

## Integration Examples

### Game Mode Startup
```cpp
void AMZGameMode::BeginPlay()
{
    Super::BeginPlay();

    UMZWorldManager* Manager = GetWorld()->GetSubsystem<UMZWorldManager>();
    AMZSpawnPoint* Spawn = Manager->GetDefaultSpawnPoint();

    if (Spawn)
    {
        GetWorld()->SpawnActor<AMZVehiclePawn>(Spawn->GetSpawnTransform());
    }
}
```

### Zone-Aware Activity
```cpp
void AMyActivity::StartInZone(AMZWorldZone* Zone)
{
    CurrentZone = Zone;
    SpeedLimit = Zone->DefaultSpeedLimit;
    CurrentSurface = Zone->DefaultSurfaceType;

    // Place activity checkpoints using zone spawn points
    TArray<AMZSpawnPoint*> Spawns;
    GetWorld()->GetSubsystem<UMZWorldManager>()->GetSpawnPointsInZone(
        Zone->ZoneID, Spawns);
}
```

## Testing Checklist

- [ ] Open L_MZ_MainWorld in editor
- [ ] Verify 5 zone actors in outliner
- [ ] Verify zone bounds visible (red boxes)
- [ ] Verify 8 spawn points in outliner
- [ ] Verify spawn point arrows/billboards visible
- [ ] Play in editor (PIE)
- [ ] Verify player spawns at ProvingGround
- [ ] Check output log for zone entry/exit messages
- [ ] Test zone queries with console commands

## Common Issues

**Commandlet doesn't create world**
- Rebuild the MileZero module first
- Check the commandlet output log for errors

**Zones not detecting vehicle**
- Verify vehicle is a Pawn actor
- Check zone bounds size (should be non-zero)
- Verify vehicle position is inside zone bounds

**Spawn points not discovered**
- Check spawn point actor label starts with "SpawnPoint_"
- Verify zone ID matches zone's ZoneID property
- Check output log for discovery messages

## Performance Notes

- Zone detection is O(n) where n = number of zones (typically < 10)
- Spawn point lookups are O(m) where m = number of spawn points (typically < 50)
- World manager ticks once per frame for zone detection
- Zone overlap detection uses UE5.7 native collision system (optimized)

## Next Steps (M6)

After M5 is verified:
1. Create activity manager using zones and spawn points
2. Implement activity system with checkpoint placement
3. Add activity-specific rules based on zone type
4. Implement save/load for world state

---

For complete documentation, see: `/docs/M5_WORLD_INFRASTRUCTURE.md`
