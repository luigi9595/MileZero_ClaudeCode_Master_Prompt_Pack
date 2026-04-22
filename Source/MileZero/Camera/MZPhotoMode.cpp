#include "MZPhotoMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "../MileZero.h"

void UMZPhotoMode::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsInPhotoMode = false;
	SavedPlayerController = nullptr;
	SavedControlledPawn = nullptr;
	bGameWasPaused = false;
	SavedGameSpeed = 1.0f;

	ResetEffects();
	ResetCamera();

	UE_LOG(LogMileZero, Log, TEXT("MZPhotoMode initialized"));
}

void UMZPhotoMode::Deinitialize()
{
	if (bIsInPhotoMode)
	{
		ExitPhotoMode();
	}

	if (PhotoModeCamera)
	{
		PhotoModeCamera = nullptr;
	}

	UE_LOG(LogMileZero, Log, TEXT("MZPhotoMode deinitialized"));

	Super::Deinitialize();
}

void UMZPhotoMode::TogglePhotoMode()
{
	if (bIsInPhotoMode)
	{
		ExitPhotoMode();
	}
	else
	{
		EnterPhotoMode();
	}
}

void UMZPhotoMode::EnterPhotoMode()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (bIsInPhotoMode)
	{
		return; // Already in photo mode
	}

	SavedPlayerController = World->GetFirstPlayerController();
	if (!SavedPlayerController)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No player controller found for photo mode"));
		return;
	}

	SavedControlledPawn = SavedPlayerController->GetPawn();
	if (SavedControlledPawn)
	{
		FVector CamLoc; FRotator CamRot;
		SavedPlayerController->PlayerCameraManager->GetCameraViewPoint(CamLoc, CamRot);
		SavedCameraLocation = CamLoc;
		SavedCameraRotation = CamRot;
	}

	// Save game state
	bGameWasPaused = World->IsPaused();
	SavedGameSpeed = World->GetWorldSettings()->TimeDilation;

	// Pause game
	if (!bGameWasPaused)
	{
		SavedPlayerController->SetPause(true);
	}

	// Setup photo mode camera
	SetupPhotoModeCamera();

	// Detach from controlled pawn
	if (SavedControlledPawn)
	{
		SavedPlayerController->SetPawn(nullptr);
		SavedPlayerController->UnPossess();
	}

	// Set new pawn with photo mode camera
	APawn* TempCameraPawn = World->SpawnActor<APawn>(SavedCameraLocation, FRotator::ZeroRotator);
	if (TempCameraPawn && PhotoModeCamera)
	{
		PhotoModeCamera->AttachToComponent(TempCameraPawn->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		SavedPlayerController->Possess(TempCameraPawn);
	}

	bIsInPhotoMode = true;
	ResetCamera();

	UE_LOG(LogMileZero, Log, TEXT("Entered photo mode"));
}

void UMZPhotoMode::ExitPhotoMode()
{
	UWorld* World = GetWorld();
	if (!World || !SavedPlayerController)
	{
		return;
	}

	if (!bIsInPhotoMode)
	{
		return; // Not in photo mode
	}

	// Restore controlled pawn
	if (SavedControlledPawn && !!IsValid(SavedControlledPawn))
	{
		SavedPlayerController->Possess(SavedControlledPawn);
	}

	// Destroy temporary camera pawn
	APawn* CurrentPawn = SavedPlayerController->GetPawn();
	if (CurrentPawn && CurrentPawn != SavedControlledPawn)
	{
		CurrentPawn->Destroy();
	}

	// Restore camera
	if (PhotoModeCamera)
	{
		PhotoModeCamera->DestroyComponent();
		PhotoModeCamera = nullptr;
	}

	// Restore game state
	if (!bGameWasPaused)
	{
		SavedPlayerController->SetPause(false);
	}

	// Restore time dilation if needed
	if (SavedGameSpeed != 1.0f)
	{
		World->GetWorldSettings()->TimeDilation = SavedGameSpeed;
	}

	bIsInPhotoMode = false;

	UE_LOG(LogMileZero, Log, TEXT("Exited photo mode"));
}

void UMZPhotoMode::SetupPhotoModeCamera()
{
	// Create a temporary camera component
	// (In practice, this would be attached to a spawned pawn)
	// For now, we just initialize the properties for later attachment
	CurrentFOV = 90.0f;
	ResetCamera();
}

void UMZPhotoMode::TakeScreenshot()
{
	if (!bIsInPhotoMode)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Cannot take screenshot outside of photo mode"));
		return;
	}

	// Use Unreal's screenshot system
	FScreenshotRequest::RequestScreenshot(false);

	UE_LOG(LogMileZero, Log, TEXT("Screenshot captured"));
}

void UMZPhotoMode::ResetEffects()
{
	Exposure = 0.0f;
	Contrast = 1.0f;
	Saturation = 1.0f;
	Vignette = 0.0f;
	bDepthOfField = false;
	FocusDistance = 1000.0f;

	UE_LOG(LogMileZero, Log, TEXT("Photo mode effects reset"));
}

void UMZPhotoMode::ResetCamera()
{
	CurrentFOV = 90.0f;
	CameraMovementSpeed = 200.0f;
	CameraFastMovementSpeed = 600.0f;
	CameraRotationSensitivity = 1.0f;

	UE_LOG(LogMileZero, Log, TEXT("Photo mode camera reset"));
}

void UMZPhotoMode::ApplyEffectsToScene()
{
	// Post-process effects would be applied here via:
	// - APostProcessVolume
	// - FPostProcessSettings
	// - Scene capture component

	// For now, this is a hook for future integration
	// Effects are stored in properties and ready for implementation
	UE_LOG(LogMileZero, Verbose, TEXT("Applying effects: Exposure=%.1f, Saturation=%.1f"), Exposure, Saturation);
}
