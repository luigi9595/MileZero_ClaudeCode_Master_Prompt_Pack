#include "MZPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MileZero.h"

AMZPlayerController::AMZPlayerController()
{
}

void AMZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// M1 fallback: start in driving mode since player spawns in vehicle
	SwitchToDrivingInput();
}

void AMZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Input action bindings are set up through Enhanced Input in the vehicle/character pawns
}

void AMZPlayerController::SwitchToOnFootInput()
{
	RemoveAllMZContexts();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (OnFootMappingContext)
		{
			Subsystem->AddMappingContext(OnFootMappingContext, 0);
		}
		if (UIMappingContext)
		{
			Subsystem->AddMappingContext(UIMappingContext, 1);
		}
	}

	bIsDriving = false;
	UE_LOG(LogMileZero, Log, TEXT("Switched to on-foot input"));
}

void AMZPlayerController::SwitchToDrivingInput()
{
	RemoveAllMZContexts();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DrivingMappingContext)
		{
			Subsystem->AddMappingContext(DrivingMappingContext, 0);
		}
		if (UIMappingContext)
		{
			Subsystem->AddMappingContext(UIMappingContext, 1);
		}
	}

	bIsDriving = true;
	UE_LOG(LogMileZero, Log, TEXT("Switched to driving input"));
}

void AMZPlayerController::RequestEnterVehicle()
{
	// Stub for M1: player starts in vehicle
	// Post-M1: find nearest vehicle, run enter animation, possess vehicle pawn
	UE_LOG(LogMileZero, Log, TEXT("RequestEnterVehicle — stubbed until on-foot character exists"));
}

void AMZPlayerController::RequestExitVehicle()
{
	// Stub for M1: player stays in vehicle
	// Post-M1: unpossess vehicle, spawn/possess character pawn at exit point
	UE_LOG(LogMileZero, Log, TEXT("RequestExitVehicle — stubbed until on-foot character exists"));
}

void AMZPlayerController::ToggleTelemetry()
{
	UE_LOG(LogMileZero, Log, TEXT("ToggleTelemetry — wired in M2"));
}

void AMZPlayerController::RemoveAllMZContexts()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (OnFootMappingContext)
		{
			Subsystem->RemoveMappingContext(OnFootMappingContext);
		}
		if (DrivingMappingContext)
		{
			Subsystem->RemoveMappingContext(DrivingMappingContext);
		}
	}
}
