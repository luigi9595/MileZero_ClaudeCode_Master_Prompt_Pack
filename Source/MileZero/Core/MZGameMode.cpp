#include "MZGameMode.h"
#include "MZPlayerController.h"
#include "MileZero/Characters/MZCharacter.h"
#include "MileZero/Vehicles/MZVehiclePawn.h"
#include "MileZero.h"
#include "UObject/ConstructorHelpers.h"

AMZGameMode::AMZGameMode()
{
	PlayerControllerClass = AMZPlayerController::StaticClass();
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
	// For M1: spawn the vehicle directly as the default pawn
	// Post-M1: spawn MZCharacter on foot, let player enter vehicles
	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}
