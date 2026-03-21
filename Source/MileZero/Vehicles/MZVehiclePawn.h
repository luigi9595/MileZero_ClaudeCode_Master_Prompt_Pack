#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "MZVehiclePawn.generated.h"

class UChaosWheeledVehicleMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UMZVehicleDataAsset;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EMZCameraMode : uint8
{
	Chase,
	Hood,
	Count UMETA(Hidden)
};

UCLASS()
class MILEZERO_API AMZVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AMZVehiclePawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

	// Apply tuning from a data asset
	UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
	void ApplyVehicleData(const UMZVehicleDataAsset* Data);

	// Reset vehicle to spawn position and orientation
	UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
	void ResetVehicle();

	// Camera cycling
	UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
	void CycleCamera();

	// Read-only telemetry accessors
	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	float GetSpeedKmh() const;

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	float GetRPM() const;

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	int32 GetCurrentGear() const;

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	float GetThrottleInput() const { return CachedThrottle; }

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	float GetBrakeInput() const { return CachedBrake; }

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	float GetSteeringInput() const { return CachedSteering; }

protected:
	// --- Components ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<USpringArmComponent> ChaseBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<UCameraComponent> ChaseCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<UCameraComponent> HoodCamera;

	// --- Enhanced Input Actions (set in editor or via defaults) ---

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Throttle;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Brake;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Steer;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Handbrake;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_ShiftUp;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_ShiftDown;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_CameraCycle;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_ResetVehicle;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Look;

	// --- Vehicle data ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UMZVehicleDataAsset> VehicleData;

private:
	// Input handlers
	void HandleThrottle(const FInputActionValue& Value);
	void HandleBrake(const FInputActionValue& Value);
	void HandleSteering(const FInputActionValue& Value);
	void HandleHandbrake(const FInputActionValue& Value);
	void HandleHandbrakeReleased(const FInputActionValue& Value);
	void HandleShiftUp();
	void HandleShiftDown();
	void HandleCameraCycle();
	void HandleResetVehicle();
	void HandleLook(const FInputActionValue& Value);

	// Camera
	void ActivateCameraMode(EMZCameraMode Mode);

	UPROPERTY()
	EMZCameraMode CurrentCameraMode = EMZCameraMode::Chase;

	// Cached inputs for telemetry
	float CachedThrottle = 0.0f;
	float CachedBrake = 0.0f;
	float CachedSteering = 0.0f;

	// Spawn transform for reset
	FTransform SpawnTransform;
};
