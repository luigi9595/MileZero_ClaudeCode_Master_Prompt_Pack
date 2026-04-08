#include "MZGameMode.h"
#include "MZGameInstance.h"
#include "MZPlayerController.h"
#include "MileZero/Characters/MZCharacter.h"
#include "MileZero/Vehicles/MZVehiclePawn.h"
#include "MileZero/Vehicles/MZVehicleRegistry.h"
#include "MileZero/Vehicles/MZVehicleDataAsset.h"
#include "MileZero/UI/MZHUD.h"
#include "MileZero.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"

AMZGameMode::AMZGameMode()
{
	PlayerControllerClass = AMZPlayerController::StaticClass();
	HUDClass = AMZHUD::StaticClass();
	// M1 fallback: player starts inside vehicle, so default pawn is the vehicle
	DefaultPawnClass = AMZVehiclePawn::StaticClass();
}

void AMZGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogMileZero, Log, TEXT("MZGameMode::InitGame — Map: %s"), *MapName);
}

void AMZGameMode::BeginPlay()
{
	Super::BeginPlay();
	EnsureTestFloor();
}

void AMZGameMode::EnsureTestFloor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Count existing static mesh actors. If none exist, the level is empty (test track not yet built)
	// and the vehicle would fall through the void — spawn a temporary floor.
	int32 ExistingMeshCount = 0;
	for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
	{
		++ExistingMeshCount;
	}

	if (ExistingMeshCount > 0)
	{
		return; // Level already has geometry
	}

	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (!PlaneMesh)
	{
		UE_LOG(LogMileZero, Warning, TEXT("EnsureTestFloor: cannot load /Engine/BasicShapes/Plane — vehicle may fall through the world"));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator, Params);
	if (Floor)
	{
		Floor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
		Floor->SetActorScale3D(FVector(500.f, 500.f, 1.f)); // 5000 UU = 50 m each side
		Floor->SetActorLabel(TEXT("TestFloor_Runtime"));
		UE_LOG(LogMileZero, Log, TEXT("EnsureTestFloor: no geometry found — spawned 50x50m runtime floor at origin"));
	}
}

APawn* AMZGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	// Get the selected vehicle ID from the game instance
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No GameInstance available"));
		return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	}

	FName SelectedVehicleID = FName("Hatch");
	if (UMZGameInstance* MZGI = Cast<UMZGameInstance>(GI))
	{
		SelectedVehicleID = MZGI->SelectedVehicleID;
	}

	// Get the vehicle registry and look up the selected vehicle data
	UMZVehicleRegistry* Registry = GI->GetSubsystem<UMZVehicleRegistry>();
	if (!Registry)
	{
		UE_LOG(LogMileZero, Warning, TEXT("MZVehicleRegistry not initialized"));
		return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	}

	UMZVehicleDataAsset* VehicleData = Registry->GetVehicleData(SelectedVehicleID);
	if (!VehicleData)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Vehicle data not found: %s, using default"), *SelectedVehicleID.ToString());
		VehicleData = Registry->GetVehicleData(Registry->GetDefaultVehicleID());
	}

	// Spawn the vehicle pawn
	AMZVehiclePawn* Vehicle = GetWorld()->SpawnActor<AMZVehiclePawn>(
		AMZVehiclePawn::StaticClass(),
		SpawnTransform
	);

	if (Vehicle)
	{
		// Apply the selected vehicle data
		if (VehicleData)
		{
			Vehicle->ApplyVehicleData(VehicleData);
			UE_LOG(LogMileZero, Log, TEXT("Spawned vehicle: %s"), *SelectedVehicleID.ToString());
		}

		// Possess the vehicle if we have a controller
		if (NewPlayer)
		{
			NewPlayer->Possess(Vehicle);
		}

		return Vehicle;
	}

	UE_LOG(LogMileZero, Error, TEXT("Failed to spawn vehicle pawn"));
	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}
