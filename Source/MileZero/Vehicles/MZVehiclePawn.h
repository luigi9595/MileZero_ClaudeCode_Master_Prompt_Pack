#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "MZVehiclePawn.generated.h"

class UChaosWheeledVehicleMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UMZVehicleDataAsset;
class UMZSurfaceContactComponent;
class UMZVehicleDamageComponent;
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

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	FName GetCurrentSurfaceID() const;

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	float GetCurrentGripMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
	float GetDamagePercent() const;

	/** Get the damage component (may be null before M4 integration) */
	UFUNCTION(BlueprintPure, Category = "MZ|Vehicle")
	UMZVehicleDamageComponent* GetDamageComponent() const { return DamageComp; }

	/** Calculate exit position relative to vehicle */
	UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
	FTransform GetExitTransform() const;

	/** Called when driver enters the vehicle */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MZ|Vehicle")
	void OnDriverEntered();

	/** Called when driver exits the vehicle */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MZ|Vehicle")
	void OnDriverExited();

protected:
	// --- Components ---

	/** Static mesh for the car body (attached to skeletal mesh) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	/** Glass mesh (attached to skeletal mesh) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UStaticMeshComponent> GlassMesh;

	/** Wheel meshes (attached to bone sockets) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelMesh_FL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelMesh_FR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelMesh_BL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UStaticMeshComponent> WheelMesh_BR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<USpringArmComponent> ChaseBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<UCameraComponent> ChaseCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<UCameraComponent> HoodCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Physics")
	TObjectPtr<UMZSurfaceContactComponent> SurfaceContact;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage")
	TObjectPtr<UMZVehicleDamageComponent> DamageComp;

	// --- Enhanced Input Actions (set in editor or via defaults) ---

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Throttle;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Brake;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_SteerLeft;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_SteerRight;

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

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_ExitVehicle;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_RadioNext;

	// Runtime-created mapping context (used when no editor assets exist)
	UPROPERTY()
	TObjectPtr<UInputMappingContext> BootstrappedMappingContext;

	// --- Vehicle data ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	TObjectPtr<UMZVehicleDataAsset> VehicleData;

	// --- Driver exit ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
	FVector ExitOffset = FVector(0.0f, -250.0f, 50.0f);

private:
	// Input handlers
	void HandleThrottle(const FInputActionValue& Value);
	void HandleBrake(const FInputActionValue& Value);
	void HandleSteerLeft(const FInputActionValue& Value);
	void HandleSteerLeftReleased(const FInputActionValue& Value);
	void HandleSteerRight(const FInputActionValue& Value);
	void HandleSteerRightReleased(const FInputActionValue& Value);
	void HandleHandbrake(const FInputActionValue& Value);
	void HandleHandbrakeReleased(const FInputActionValue& Value);
	void HandleShiftUp();
	void HandleShiftDown();
	void HandleCameraCycle();
	void HandleResetVehicle();
	void HandleLook(const FInputActionValue& Value);
	void HandleExitVehicle();
	void HandleRadioNext();

	// Bootstrap input if no editor assets assigned
	void BootstrapDefaultInput();

	// Camera
	void ActivateCameraMode(EMZCameraMode Mode);

	// Collision handler for damage
	UFUNCTION()
	void OnVehicleHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	// Apply damage effects (steering pull, power loss, drag) each tick
	void ApplyDamageEffects(float DeltaTime);

	UPROPERTY()
	EMZCameraMode CurrentCameraMode = EMZCameraMode::Chase;

	// Cached inputs for telemetry
	float CachedThrottle = 0.0f;
	float CachedBrake = 0.0f;
	float CachedSteering = 0.0f;
	bool bSteerLeftHeld = false;
	bool bSteerRightHeld = false;

	// Spawn transform for reset
	FTransform SpawnTransform;
};
