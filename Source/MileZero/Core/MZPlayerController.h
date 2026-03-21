#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MZPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class MILEZERO_API AMZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMZPlayerController();

	// Called to switch between on-foot and driving input contexts
	void SwitchToOnFootInput();
	void SwitchToDrivingInput();

	// Vehicle enter/exit request
	void RequestEnterVehicle();
	void RequestExitVehicle();

	// Telemetry toggle
	void ToggleTelemetry();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Enhanced Input mapping contexts — set in editor or via code
	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputMappingContext> OnFootMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputMappingContext> DrivingMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
	TObjectPtr<UInputMappingContext> UIMappingContext;

private:
	void RemoveAllMZContexts();

	bool bIsDriving = false;
};
