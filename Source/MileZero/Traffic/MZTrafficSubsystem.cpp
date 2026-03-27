#include "MZTrafficSubsystem.h"
#include "MileZero.h"
#include "MZTrafficVehicle.h"
#include "MZTrafficPath.h"
#include "Vehicles/MZVehiclePawn.h"
#include "EngineUtils.h"

void UMZTrafficSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Set default traffic vehicle class if not assigned
	if (!TrafficVehicleClass)
	{
		TrafficVehicleClass = AMZTrafficVehicle::StaticClass();
	}

	UE_LOG(LogMileZero, Log, TEXT("MZTrafficSubsystem initialized. Max vehicles: %d"), MaxTrafficVehicles);
}

void UMZTrafficSubsystem::Deinitialize()
{
	DespawnAllTraffic();
	Super::Deinitialize();
}

void UMZTrafficSubsystem::Tick(float DeltaTime)
{
	// Accumulate time and perform management at intervals
	ManagementTickAccumulator += DeltaTime;

	if (ManagementTickAccumulator >= ManagementTickInterval)
	{
		ManagementTickAccumulator = 0.0f;

		// Despawn vehicles that are too far
		DespawnFarVehicles();

		// Spawn new vehicles if below max
		if (ActiveTraffic.Num() < MaxTrafficVehicles)
		{
			SpawnTraffic();
		}

		// Remove null entries from active traffic
		ActiveTraffic.RemoveAll([](const AMZTrafficVehicle* Vehicle)
		{
			return !IsValid(Vehicle);
		});
	}
}

int32 UMZTrafficSubsystem::GetNearbyTrafficCount(FVector Location, float Radius) const
{
	int32 Count = 0;

	for (const AMZTrafficVehicle* Vehicle : ActiveTraffic)
	{
		if (IsValid(Vehicle))
		{
			float Distance = FVector::Dist(Vehicle->GetActorLocation(), Location);
			if (Distance <= Radius)
			{
				++Count;
			}
		}
	}

	return Count;
}

void UMZTrafficSubsystem::SetTrafficDensity(float Density01)
{
	Density01 = FMath::Clamp(Density01, 0.0f, 1.0f);
	MaxTrafficVehicles = FMath::RoundToInt(Density01 * 50.0f); // Scale to 0-50 max
	UE_LOG(LogMileZero, Log, TEXT("Traffic density set to %.2f (max vehicles: %d)"), Density01, MaxTrafficVehicles);
}

float UMZTrafficSubsystem::GetTrafficDensity() const
{
	return static_cast<float>(ActiveTraffic.Num()) / static_cast<float>(MaxTrafficVehicles);
}

AMZTrafficVehicle* UMZTrafficSubsystem::SpawnTrafficVehicle(AMZTrafficPath* Path, FVector SpawnLocation)
{
	if (!IsValid(Path) || !IsValid(GetWorld()))
	{
		return nullptr;
	}

	FVector ActualSpawnLocation = Path ? Path->GetClosestLocationOnPath(SpawnLocation) : SpawnLocation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMZTrafficVehicle* NewVehicle = GetWorld()->SpawnActor<AMZTrafficVehicle>(
		TrafficVehicleClass,
		ActualSpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams);

	if (NewVehicle)
	{
		NewVehicle->SetPath(Path);
		ActiveTraffic.Add(NewVehicle);
		UE_LOG(LogMileZero, Log, TEXT("Spawned traffic vehicle %s on path %s"), *NewVehicle->GetName(), *Path->GetName());
	}

	return NewVehicle;
}

void UMZTrafficSubsystem::DespawnAllTraffic()
{
	for (AMZTrafficVehicle* Vehicle : ActiveTraffic)
	{
		if (IsValid(Vehicle))
		{
			Vehicle->Destroy();
		}
	}
	ActiveTraffic.Empty();
	UE_LOG(LogMileZero, Log, TEXT("All traffic vehicles despawned"));
}

void UMZTrafficSubsystem::GetAvailableTrafficPaths(TArray<AMZTrafficPath*>& OutPaths) const
{
	OutPaths.Empty();

	for (TActorIterator<AMZTrafficPath> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->IsPathValid())
		{
			OutPaths.Add(*ActorItr);
		}
	}
}

void UMZTrafficSubsystem::SpawnTraffic()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	FVector PlayerLocation = GetPlayerLocation();
	if (PlayerLocation == FVector::ZeroVector)
	{
		return; // No player yet
	}

	// Rebuild paths cache if needed
	if (bNeedPathsCached)
	{
		CachedTrafficPaths.Empty();
		for (TActorIterator<AMZTrafficPath> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->IsPathValid())
			{
				CachedTrafficPaths.Add(*ActorItr);
			}
		}
		bNeedPathsCached = false;

		if (CachedTrafficPaths.Num() == 0)
		{
			UE_LOG(LogMileZero, Warning, TEXT("No valid traffic paths found in world"));
			return;
		}
	}

	// Try to spawn a vehicle
	int32 AttemptCount = 0;
	while (ActiveTraffic.Num() < MaxTrafficVehicles && AttemptCount < 5)
	{
		++AttemptCount;

		// Pick a random path
		AMZTrafficPath* TargetPath = nullptr;
		for (const TWeakObjectPtr<AMZTrafficPath>& WeakPath : CachedTrafficPaths)
		{
			if (WeakPath.IsValid())
			{
				TargetPath = WeakPath.Get();
				break;
			}
		}

		if (!TargetPath)
		{
			bNeedPathsCached = true; // Refresh cache
			break;
		}

		// Find a spawn location on the path
		FVector RandomOnPath = TargetPath->GetRandomStartLocation();
		float DistFromPlayer = FVector::Dist(RandomOnPath, PlayerLocation);

		// Check if spawn location is in valid range
		if (DistFromPlayer >= MinSpawnDistance && DistFromPlayer <= SpawnRadius)
		{
			SpawnTrafficVehicle(TargetPath, RandomOnPath);
			break;
		}
	}
}

void UMZTrafficSubsystem::DespawnFarVehicles()
{
	FVector PlayerLocation = GetPlayerLocation();

	for (int32 i = ActiveTraffic.Num() - 1; i >= 0; --i)
	{
		AMZTrafficVehicle* Vehicle = ActiveTraffic[i];

		if (!IsValid(Vehicle))
		{
			ActiveTraffic.RemoveAt(i);
			continue;
		}

		float Distance = FVector::Dist(Vehicle->GetActorLocation(), PlayerLocation);

		if (Distance > DespawnRadius)
		{
			Vehicle->Destroy();
			ActiveTraffic.RemoveAt(i);
			UE_LOG(LogMileZero, Log, TEXT("Despawned traffic vehicle %s (distance: %.0f)"), *Vehicle->GetName(), Distance);
		}
	}
}

FVector UMZTrafficSubsystem::GetPlayerLocation() const
{
	if (!CachedPlayerVehicle.IsValid())
	{
		// Try to find player vehicle
		for (TActorIterator<AMZVehiclePawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			CachedPlayerVehicle = *ActorItr;
			break;
		}
	}

	if (CachedPlayerVehicle.IsValid())
	{
		return CachedPlayerVehicle->GetActorLocation();
	}

	return FVector::ZeroVector;
}

AMZTrafficPath* UMZTrafficSubsystem::GetRandomTrafficPath()
{
	if (CachedTrafficPaths.Num() == 0)
	{
		return nullptr;
	}

	int32 RandomIndex = FMath::RandRange(0, CachedTrafficPaths.Num() - 1);
	return CachedTrafficPaths[RandomIndex].Get();
}
