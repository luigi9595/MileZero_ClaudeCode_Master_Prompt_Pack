#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MZCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UInputAction;
class AMZVehiclePawn;
struct FInputActionValue;

UCLASS()
class MILEZERO_API AMZCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMZCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Interaction")
	TObjectPtr<USphereComponent> VehicleDetectionSphere;

	// Enhanced Input actions
	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Interact;

	// Vehicle entry
	UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
	void EnterVehicle();

public:
	UFUNCTION(BlueprintPure, Category = "MZ|Vehicle")
	AMZVehiclePawn* GetNearbyVehicle() const { return NearbyVehicle; }

private:
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJump();
	void HandleInteract();

	UFUNCTION()
	void OnVehicleDetectionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnVehicleDetectionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "MZ|Vehicle")
	TObjectPtr<AMZVehiclePawn> NearbyVehicle;
};
