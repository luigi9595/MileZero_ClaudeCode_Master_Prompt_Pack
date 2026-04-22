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
#include "TimerManager.h"
#include "Engine/World.h"

AMZGameMode::AMZGameMode()
{
	PlayerControllerClass = AMZPlayerController::StaticClass();
	HUDClass = AMZHUD::StaticClass();
	// Player starts as on-foot character, can enter vehicle
	DefaultPawnClass = AMZCharacter::StaticClass();
}

void AMZGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogMileZero, Log, TEXT("MZGameMode::InitGame — Map: %s"), *MapName);
}

APawn* AMZGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	// Spawn the character pawn
	AMZCharacter* Character = GetWorld()->SpawnActor<AMZCharacter>(
		AMZCharacter::StaticClass(),
		SpawnTransform
	);

	if (!Character)
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to spawn character pawn"));
		return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	}

	SpawnedCharacter = Character;

	// Possess the character
	if (NewPlayer)
	{
		NewPlayer->Possess(Character);
	}

	UE_LOG(LogMileZero, Log, TEXT("Spawned player character"));
	return Character;
}

void AMZGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Spawn a vehicle nearby for the player to enter
	if (!SpawnedCharacter)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No spawned character found for vehicle setup"));
		return;
	}

	// Get the selected vehicle ID from the game instance
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No GameInstance available"));
		return;
	}

	FName SelectedVehicleID = FName("Hatch");
	if (UMZGameInstance* MZGI = Cast<UMZGameInstance>(GI))
	{
		SelectedVehicleID = MZGI->SelectedVehicleID;
	}

	// Get the vehicle registry
	UMZVehicleRegistry* Registry = GI->GetSubsystem<UMZVehicleRegistry>();
	if (!Registry)
	{
		UE_LOG(LogMileZero, Warning, TEXT("MZVehicleRegistry not initialized"));
		return;
	}

	UMZVehicleDataAsset* VehicleData = Registry->GetVehicleData(SelectedVehicleID);
	if (!VehicleData)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Vehicle data not found: %s, using default"), *SelectedVehicleID.ToString());
		VehicleData = Registry->GetVehicleData(Registry->GetDefaultVehicleID());
	}

	// Calculate vehicle spawn position: offset from character
	FVector VehicleSpawnPos = SpawnedCharacter->GetActorLocation() + FVector(500.0f, 0.0f, 0.0f);
	FTransform VehicleSpawnTransform(SpawnedCharacter->GetActorRotation(), VehicleSpawnPos);

	// Spawn the vehicle
	AMZVehiclePawn* Vehicle = GetWorld()->SpawnActor<AMZVehiclePawn>(
		AMZVehiclePawn::StaticClass(),
		VehicleSpawnTransform
	);

	if (Vehicle)
	{
		if (VehicleData)
		{
			Vehicle->ApplyVehicleData(VehicleData);
			UE_LOG(LogMileZero, Log, TEXT("Spawned vehicle: %s"), *SelectedVehicleID.ToString());
		}

		// Auto-enter the vehicle: directly possess it (bypasses proximity check)
		if (APlayerController* PC = Cast<APlayerController>(SpawnedCharacter->GetController()))
		{
			// Hide and disable the character
			SpawnedCharacter->SetActorHiddenInGame(true);
			SpawnedCharacter->SetActorEnableCollision(false);
			
			// Store character reference in controller
			if (AMZPlayerController* MZController = Cast<AMZPlayerController>(PC))
			{
				MZController->SetStoredCharacter(SpawnedCharacter);
			}
			
			// Directly possess the vehicle
			PC->UnPossess();
			PC->Possess(Vehicle);
			Vehicle->OnDriverEntered();
			UE_LOG(LogMileZero, Log, TEXT("Auto-possessed vehicle (direct)"));
		}
	}
	else
	{
		UE_LOG(LogMileZero, Warning, TEXT("Failed to spawn vehicle"));
	}
}
