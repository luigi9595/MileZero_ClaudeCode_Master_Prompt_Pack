#include "MZPlayerController.h"
#include "MileZero/Characters/MZCharacter.h"
#include "MileZero/Vehicles/MZVehiclePawn.h"
#include "MileZero/UI/MZPauseManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "MileZero.h"
#include "Kismet/GameplayStatics.h"

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
	// Get the current pawn (should be the character)
	APawn* CurrentPawn = GetPawn();
	AMZCharacter* MZChar = Cast<AMZCharacter>(CurrentPawn);
	if (!MZChar)
	{
		UE_LOG(LogMileZero, Warning, TEXT("RequestEnterVehicle: not possessing a character"));
		return;
	}

	// Get the nearby vehicle
	AMZVehiclePawn* Vehicle = MZChar->GetNearbyVehicle();
	if (!Vehicle)
	{
		UE_LOG(LogMileZero, Warning, TEXT("RequestEnterVehicle: no nearby vehicle"));
		return;
	}

	// Store reference to character for later exit
	StoredCharacter = MZChar;
	LastVehicle = Vehicle;

	// Hide the character
	MZChar->SetActorHiddenInGame(true);
	MZChar->SetActorEnableCollision(false);

	// Unpossess character and possess vehicle
	UnPossess();
	Possess(Vehicle);

	// Call vehicle event
	Vehicle->OnDriverEntered();

	// Switch to driving input
	SwitchToDrivingInput();

	UE_LOG(LogMileZero, Log, TEXT("Entered vehicle: %s"), *Vehicle->GetName());
}

void AMZPlayerController::RequestExitVehicle()
{
	// Get the current pawn (should be the vehicle)
	APawn* CurrentPawn = GetPawn();
	AMZVehiclePawn* Vehicle = Cast<AMZVehiclePawn>(CurrentPawn);
	if (!Vehicle)
	{
		UE_LOG(LogMileZero, Warning, TEXT("RequestExitVehicle: not possessing a vehicle"));
		return;
	}

	// Check speed — can't exit while moving
	float SpeedKmh = FMath::Abs(Vehicle->GetSpeedKmh());
	if (SpeedKmh > 5.0f)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Cannot exit vehicle while moving (%.1f km/h)"), SpeedKmh);
		return;
	}

	// Get the stored character
	if (!StoredCharacter)
	{
		UE_LOG(LogMileZero, Warning, TEXT("RequestExitVehicle: no stored character"));
		return;
	}

	// Calculate exit position
	FTransform ExitTransform = Vehicle->GetExitTransform();

	// Unpossess vehicle
	UnPossess();

	// Place character at exit position and show it
	StoredCharacter->SetActorTransform(ExitTransform);
	StoredCharacter->SetActorHiddenInGame(false);
	StoredCharacter->SetActorEnableCollision(true);

	// Possess character
	Possess(StoredCharacter);

	// Call vehicle event
	Vehicle->OnDriverExited();

	// Switch to on-foot input
	SwitchToOnFootInput();

	UE_LOG(LogMileZero, Log, TEXT("Exited vehicle: %s"), *Vehicle->GetName());
}

void AMZPlayerController::ToggleTelemetry()
{
	UE_LOG(LogMileZero, Log, TEXT("ToggleTelemetry — wired in M2"));
}

void AMZPlayerController::HandlePause()
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(GetWorld()))
	{
		if (UMZPauseManager* PauseMgr = GI->GetSubsystem<UMZPauseManager>())
		{
			PauseMgr->TogglePause();
		}
	}
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
