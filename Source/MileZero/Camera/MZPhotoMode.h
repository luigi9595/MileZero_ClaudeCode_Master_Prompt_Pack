#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZPhotoMode.generated.h"

class APlayerController;
class UCameraComponent;
class APawn;

/**
 * Game instance subsystem managing photo mode.
 * Allows free-fly camera, effect editing, and screenshot capture.
 * Pauses game when enabled.
 */
UCLASS()
class MILEZERO_API UMZPhotoMode : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- State ---

	UPROPERTY(BlueprintReadOnly, Category = "MZ|PhotoMode")
	bool bIsInPhotoMode = false;

	// --- Effects (Tuning) ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Effects", meta = (ClampMin = "-2.0", ClampMax = "2.0"))
	float Exposure = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Effects", meta = (ClampMin = "0.5", ClampMax = "2.0"))
	float Contrast = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Effects", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Saturation = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Effects", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Vignette = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Effects")
	bool bDepthOfField = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Effects", meta = (ClampMin = "10.0", ClampMax = "10000.0"))
	float FocusDistance = 1000.0f;

	// --- Camera Control (in photo mode) ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Camera", meta = (ClampMin = "60.0", ClampMax = "120.0"))
	float CurrentFOV = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Camera")
	float CameraMovementSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Camera")
	float CameraFastMovementSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|PhotoMode|Camera")
	float CameraRotationSensitivity = 1.0f;

	// --- API ---

	/**
	 * Toggle photo mode on/off
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|PhotoMode")
	void TogglePhotoMode();

	/**
	 * Enter photo mode
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|PhotoMode")
	void EnterPhotoMode();

	/**
	 * Exit photo mode and restore gameplay
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|PhotoMode")
	void ExitPhotoMode();

	/**
	 * Capture screenshot
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|PhotoMode")
	void TakeScreenshot();

	/**
	 * Reset all effects to defaults
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|PhotoMode")
	void ResetEffects();

	/**
	 * Reset camera to default state
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|PhotoMode")
	void ResetCamera();

protected:
	// --- Internal State ---

	TObjectPtr<APlayerController> SavedPlayerController;
	TObjectPtr<APawn> SavedControlledPawn;
	FVector SavedCameraLocation;
	FRotator SavedCameraRotation;
	float SavedGameSpeed;
	bool bGameWasPaused;

	TObjectPtr<UCameraComponent> PhotoModeCamera;

	// --- Internal Methods ---

	void SetupPhotoModeCamera();
	void ApplyEffectsToScene();
};
