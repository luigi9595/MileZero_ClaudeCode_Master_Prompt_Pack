#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MZSaveGame.generated.h"

UCLASS()
class MILEZERO_API UMZSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// Vehicle the player was using
	UPROPERTY()
	FName LastVehicleID;

	// Player location in world
	UPROPERTY()
	FTransform PlayerTransform;

	// Completed activities
	UPROPERTY()
	TArray<FName> CompletedActivities;

	// Save slot name constant
	static const FString DefaultSlotName;
};
