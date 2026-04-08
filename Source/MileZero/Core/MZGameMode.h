#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MZGameMode.generated.h"

class AMZHUD;
class UMZGameInstance;

UCLASS()
class MILEZERO_API AMZGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMZGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;

protected:
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

private:
	// Spawns a large floor plane at runtime if the level has no static mesh geometry.
	// Prevents the vehicle from falling through the void in empty test levels.
	void EnsureTestFloor();
};
