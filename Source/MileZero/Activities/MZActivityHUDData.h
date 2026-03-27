#pragma once

#include "CoreMinimal.h"
#include "MZActivityDataAsset.h"
#include "MZActivityHUDData.generated.h"

/**
 * Simple data structure for passing activity state to HUD widget
 */
USTRUCT(BlueprintType)
struct FMZActivityHUDData
{
	GENERATED_BODY()

	// Current objective/milestone text
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	FText ObjectiveText;

	// Elapsed time in seconds
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	float ElapsedTime = 0.0f;

	// Time limit in seconds (0 = no limit)
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	float TimeLimit = 0.0f;

	// Current checkpoint index (0-based)
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	int32 CurrentCheckpoint = 0;

	// Total number of checkpoints
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	int32 TotalCheckpoints = 0;

	// Accumulated damage as percentage (0-100)
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	float DamagePercent = 0.0f;

	// Activity type
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	EMZActivityType ActivityType = EMZActivityType::CheckpointSprint;

	// Whether an activity is currently running
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	bool bIsActive = false;
};
