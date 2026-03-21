#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MZCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
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

	// Enhanced Input actions
	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputAction> IA_Interact;

private:
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJump();
	void HandleInteract();
};
