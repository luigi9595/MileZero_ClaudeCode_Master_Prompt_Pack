#include "MZWorldManager.h"
#include "MileZero.h"
#include "MZWorldZone.h"
#include "MZSpawnPoint.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"

void UMZWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogMileZero, Display, TEXT("=== MZWorldManager Initializing ==="));

	// Discover zones and spawn points
	DiscoverZonesAndSpawnPoints();

	UE_LOG(LogMileZero, Display, TEXT("MZWorldManager ready: %d zones, %d spawn points"),
	       AllZones.Num(), AllSpawnPoints.Num());
}

void UMZWorldManager::Tick(float DeltaTime)
{
	// Only update player zone if we have a valid world
	if (!GetWorld())
	{
		return;
	}

	UpdatePlayerZone();
}

bool UMZWorldManager::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	// Support editor and game worlds, but not preview
	return WorldType == EWorldType::Editor || WorldType == EWorldType::Game;
}

AMZSpawnPoint* UMZWorldManager::GetNearestSpawnPoint(const FVector& Location) const
{
	if (AllSpawnPoints.Num() == 0)
	{
		return nullptr;
	}

	float NearestDistance = MAX_FLT;
	AMZSpawnPoint* NearestSpawn = nullptr;

	for (AMZSpawnPoint* SpawnPoint : AllSpawnPoints)
	{
		if (!SpawnPoint)
		{
			continue;
		}

		float Distance = FVector::Distance(Location, SpawnPoint->SpawnLocation);
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestSpawn = SpawnPoint;
		}
	}

	return NearestSpawn;
}

AMZSpawnPoint* UMZWorldManager::GetDefaultSpawnPoint() const
{
	for (AMZSpawnPoint* SpawnPoint : AllSpawnPoints)
	{
		if (SpawnPoint && SpawnPoint->bIsDefaultSpawn)
		{
			return SpawnPoint;
		}
	}

	// Fallback: return first spawn point
	if (AllSpawnPoints.Num() > 0)
	{
		return AllSpawnPoints[0];
	}

	return nullptr;
}

AMZWorldZone* UMZWorldManager::GetZoneByID(const FName& ZoneID) const
{
	for (AMZWorldZone* Zone : AllZones)
	{
		if (Zone && Zone->ZoneID == ZoneID)
		{
			return Zone;
		}
	}

	return nullptr;
}

void UMZWorldManager::GetAllZonesOfType(EMZZoneType ZoneType, TArray<AMZWorldZone*>& OutZones) const
{
	OutZones.Empty();

	for (AMZWorldZone* Zone : AllZones)
	{
		if (Zone && Zone->ZoneType == ZoneType)
		{
			OutZones.Add(Zone);
		}
	}
}

AMZSpawnPoint* UMZWorldManager::GetRandomSpawnPointInZone(const FName& ZoneID) const
{
	TArray<AMZSpawnPoint*> ZoneSpawns;
	GetSpawnPointsInZone(ZoneID, ZoneSpawns);

	if (ZoneSpawns.Num() == 0)
	{
		return nullptr;
	}

	int32 RandomIndex = FMath::RandRange(0, ZoneSpawns.Num() - 1);
	return ZoneSpawns[RandomIndex];
}

void UMZWorldManager::GetSpawnPointsInZone(const FName& ZoneID, TArray<AMZSpawnPoint*>& OutSpawnPoints) const
{
	OutSpawnPoints.Empty();

	for (AMZSpawnPoint* SpawnPoint : AllSpawnPoints)
	{
		if (SpawnPoint && SpawnPoint->AssociatedZoneID == ZoneID)
		{
			OutSpawnPoints.Add(SpawnPoint);
		}
	}
}

FString UMZWorldManager::GetWorldManagerInfo() const
{
	FString CurrentZoneName = CurrentZone ? CurrentZone->ZoneID.ToString() : TEXT("(none)");
	return FString::Printf(TEXT("WorldManager: Current Zone=%s | Total Zones=%d | Total Spawns=%d"),
	                        *CurrentZoneName, AllZones.Num(), AllSpawnPoints.Num());
}

void UMZWorldManager::DiscoverZonesAndSpawnPoints()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogMileZero, Error, TEXT("MZWorldManager: No valid world for discovery"));
		return;
	}

	AllZones.Empty();
	AllSpawnPoints.Empty();

	// Find all zone actors
	for (TActorIterator<AMZWorldZone> It(World); It; ++It)
	{
		AMZWorldZone* Zone = *It;
		if (Zone && !Zone->IsTemplate())
		{
			AllZones.Add(Zone);
			UE_LOG(LogMileZero, Display, TEXT("  Found Zone: %s [%s]"), *Zone->ZoneID.ToString(), *Zone->ZoneName.ToString());
		}
	}

	// Find all spawn point actors
	for (TActorIterator<AMZSpawnPoint> It(World); It; ++It)
	{
		AMZSpawnPoint* SpawnPoint = *It;
		if (SpawnPoint && !SpawnPoint->IsTemplate())
		{
			AllSpawnPoints.Add(SpawnPoint);
			UE_LOG(LogMileZero, Display, TEXT("  Found SpawnPoint: %s (Zone: %s, Default: %s)"),
			       *SpawnPoint->GetActorLabel(), *SpawnPoint->AssociatedZoneID.ToString(),
			       SpawnPoint->bIsDefaultSpawn ? TEXT("Yes") : TEXT("No"));
		}
	}

	UE_LOG(LogMileZero, Display, TEXT("Discovery complete: %d zones, %d spawn points"), AllZones.Num(), AllSpawnPoints.Num());
}

void UMZWorldManager::UpdatePlayerZone()
{
	// Get the player pawn
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{
		CachedPlayerPawn.Reset();
		return;
	}

	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	// Find the zone containing the player
	AMZWorldZone* NewZone = nullptr;
	for (AMZWorldZone* Zone : AllZones)
	{
		if (Zone && Zone->ZoneBounds && Zone->ZoneBounds->IsOverlappingActor(PlayerPawn))
		{
			NewZone = Zone;
			break;
		}
	}

	// Check if zone changed
	if (NewZone != CurrentZone)
	{
		PreviousZone = CurrentZone;
		CurrentZone = NewZone;

		if (CurrentZone)
		{
			UE_LOG(LogMileZero, Display, TEXT("Player entered zone: %s"), *CurrentZone->ZoneID.ToString());
		}
		else
		{
			UE_LOG(LogMileZero, Verbose, TEXT("Player is between zones"));
		}

		OnZoneChanged.Broadcast(PreviousZone, NewZone);
	}

	CachedPlayerPawn = PlayerPawn;
}
