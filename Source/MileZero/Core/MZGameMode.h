#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MZGameMode.generated.h"

UCLASS()
class MILEZERO_API AMZGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMZGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
};
