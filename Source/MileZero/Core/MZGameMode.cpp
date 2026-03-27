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
