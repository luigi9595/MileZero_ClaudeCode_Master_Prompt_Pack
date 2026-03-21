#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MZGameInstance.generated.h"

UCLASS()
class MILEZERO_API UMZGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	// Current vehicle selection tag for spawn
	UPROPERTY(BlueprintReadWrite, Category = "MZ|Game")
	FName SelectedVehicleID = FName("Hatch");
};
