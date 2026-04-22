#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Damage/MZDamageTypes.h"
#include "Economy/MZEconomyTypes.h"
#include "MZSaveGame.generated.h"

/**
 * Player settings/preferences that can be saved
 */
USTRUCT(BlueprintType)
struct FMZPlayerSettings
{
	GENERATED_BODY()

	UPROPERTY()
	float MusicVolume = 1.0f;

	UPROPERTY()
	float SFXVolume = 1.0f;

	UPROPERTY()
	bool bShowTelemetry = false;
};

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

	// Best times for each activity (ActivityID -> Time in seconds)
	UPROPERTY()
	TMap<FName, float> BestTimes;

	// Total playtime in seconds
	UPROPERTY()
	float TotalPlayTimeSeconds = 0.0f;

	// Current map the save was made in
	UPROPERTY()
	FString CurrentMapName;

	// Vehicle damage state
	UPROPERTY()
	FMZDamageState DamageState;

	// Player settings
	UPROPERTY()
	FMZPlayerSettings PlayerSettings;

	// Current player money
	UPROPERTY()
	float CurrentMoney = 5000.0f;

	// Player statistics (distance, playtime, activities, crashes, skill, etc.)
	UPROPERTY()
	FMZPlayerStats PlayerStats;

	// Recent transaction history (last 50 transactions)
	UPROPERTY()
	TArray<FMZTransaction> RecentTransactions;

	// Save slot name constant
	static const FString DefaultSlotName;
};
