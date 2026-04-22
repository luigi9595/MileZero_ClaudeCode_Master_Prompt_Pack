#include "MZPedestrianSubsystem.h"
#include "MZPedestrianActor.h"
#include "../World/MZWorldManager.h"
#include "../MileZero.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void UMZPedestrianSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ManagementTickCounter = 0.0f;

	// Try to get world manager reference
	UWorld* World = GetWorld();
	if (World)
	{
		WorldManager = World->GetSubsystem<UMZWorldManager>();
	}

	UE_LOG(LogMileZero, Log, TEXT("MZPedestrianSubsystem initialized"));
}

void UMZPedestrianSubsystem::Deinitialize()
{
	// Despawn all pedestrians
	for (AMZPedestrianActor* Ped : ActivePedestrians)
	{
		if (Ped && !!IsValid(Ped))
		{
			Ped->Destroy();
		}
	}
	ActivePedestrians.Empty();

	UE_LOG(LogMileZero, Log, TEXT("MZPedestrianSubsystem deinitialized"));

	Super::Deinitialize();
}

void UMZPedestrianSubsystem::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Periodic spawn/despawn management
	ManagementTickCounter += DeltaTime;
	if (ManagementTickCounter >= ManagementTickInterval)
	{
		ManagementTickCounter = 0.0f;

		SpawnPedestriansAroundPlayer();
		DespawnFarPedestrians();
	}
}

void UMZPedestrianSubsystem::SpawnPedestriansAroundPlayer()
{
	UWorld* World = GetWorld();
	if (!World || !PedestrianActorClass)
	{
		return;
	}

	// Get player pawn
	APawn* PlayerPawn = nullptr;
	if (World->GetFirstPlayerController())
	{
		PlayerPawn = World->GetFirstPlayerController()->GetPawn();
	}

	if (!PlayerPawn)
	{
		return;
	}

	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	// Clean up destroyed pedestrians
	ActivePedestrians.RemoveAll([](const TObjectPtr<AMZPedestrianActor>& Ped)
	{
		return !Ped || !IsValid(Ped);
	});

	// Determine target count based on current zone
	FName CurrentZoneID = FName("City"); // Default fallback
	if (WorldManager)
	{
		// Query which zone player is in (for now use a simple fallback)
		CurrentZoneID = FName("City");
	}

	int32 ZoneDensityLimit = GetZoneDensityLimit(CurrentZoneID);
	int32 CurrentInZone = GetPedestrianCountInZone(CurrentZoneID);

	// Spawn new pedestrians if under limit
	int32 TotalActive = ActivePedestrians.Num();
	if (TotalActive < MaxPedestrians && CurrentInZone < ZoneDensityLimit)
	{
		// Try to spawn a few pedestrians around the player
		int32 SpawnCount = FMath::Min(3, MaxPedestrians - TotalActive);

		for (int32 i = 0; i < SpawnCount; ++i)
		{
			// Random spawn location in spawn radius, away from player
			float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);
			float RandomDistance = FMath::RandRange(MinSpawnDistance, SpawnRadius);

			FVector SpawnOffset = FVector(
				FMath::Cos(RandomAngle) * RandomDistance,
				FMath::Sin(RandomAngle) * RandomDistance,
				0.0f
			);

			FVector SpawnLocation = PlayerLocation + SpawnOffset;

			AMZPedestrianActor* NewPed = SpawnPedestrianAtLocation(SpawnLocation);
			if (NewPed)
			{
				ActivePedestrians.Add(NewPed);
				UE_LOG(LogMileZero, Log, TEXT("Spawned pedestrian at %s (total: %d)"), *SpawnLocation.ToString(), ActivePedestrians.Num());
			}
		}
	}
}

void UMZPedestrianSubsystem::DespawnFarPedestrians()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Get player pawn
	APawn* PlayerPawn = nullptr;
	if (World->GetFirstPlayerController())
	{
		PlayerPawn = World->GetFirstPlayerController()->GetPawn();
	}

	if (!PlayerPawn)
	{
		return;
	}

	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	// Remove pedestrians outside despawn radius
	for (int32 i = ActivePedestrians.Num() - 1; i >= 0; --i)
	{
		AMZPedestrianActor* Ped = ActivePedestrians[i];
		if (!Ped || !IsValid(Ped))
		{
			ActivePedestrians.RemoveAt(i);
			continue;
		}

		float Distance = FVector::Dist(Ped->GetActorLocation(), PlayerLocation);
		if (Distance > DespawnRadius)
		{
			Ped->Destroy();
			ActivePedestrians.RemoveAt(i);
			UE_LOG(LogMileZero, Log, TEXT("Despawned pedestrian at distance %.0f"), Distance);
		}
	}
}

int32 UMZPedestrianSubsystem::GetZoneDensityLimit(FName ZoneID) const
{
	if (ZoneID == FName("City"))
	{
		return FMath::Max(1, FMath::RoundToInt(MaxPedestrians * CityDensity));
	}
	else if (ZoneID == FName("Industrial"))
	{
		return FMath::Max(1, FMath::RoundToInt(MaxPedestrians * IndustrialDensity));
	}
	else if (ZoneID == FName("Rural"))
	{
		return FMath::Max(1, FMath::RoundToInt(MaxPedestrians * RuralDensity));
	}
	else if (ZoneID == FName("Highway") || ZoneID == FName("ProvingGround"))
	{
		return 0; // No pedestrians in highway or proving ground
	}

	return FMath::Max(1, FMath::RoundToInt(MaxPedestrians * 0.5f)); // Default
}

int32 UMZPedestrianSubsystem::GetPedestrianCountInZone(FName ZoneID) const
{
	int32 Count = 0;

	for (const AMZPedestrianActor* Ped : ActivePedestrians)
	{
		if (Ped && !!IsValid(Ped))
		{
			// Simple zone check: assume all active pedestrians are in current zone for now
			// In a full implementation, query WorldManager to determine actual zone
			Count++;
		}
	}

	return Count;
}

AMZPedestrianActor* UMZPedestrianSubsystem::SpawnPedestrianAtLocation(FVector Location)
{
	UWorld* World = GetWorld();
	if (!World || !PedestrianActorClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AMZPedestrianActor* NewPed = World->SpawnActor<AMZPedestrianActor>(
		PedestrianActorClass,
		Location,
		FRotator::ZeroRotator,
		SpawnParams
	);

	return NewPed;
}

void UMZPedestrianSubsystem::OnPedestrianHitByVehicle(AMZPedestrianActor* Pedestrian, FVector ImpactLocation)
{
	if (!Pedestrian || !IsValid(Pedestrian))
	{
		return;
	}

	// Trigger fleeing behavior
	Pedestrian->OnHitByVehicle(ImpactLocation);

	UE_LOG(LogMileZero, Log, TEXT("Pedestrian hit by vehicle at %s"), *ImpactLocation.ToString());
}
