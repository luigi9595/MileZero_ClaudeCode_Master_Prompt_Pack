#pragma once

#include "CoreMinimal.h"
#include "MZEconomyTypes.generated.h"

/**
 * Represents a single economic transaction (money in/out)
 */
USTRUCT(BlueprintType)
struct FMZTransaction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Economy")
	float Amount = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Economy")
	FString Reason;

	UPROPERTY(BlueprintReadOnly, Category = "Economy")
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadOnly, Category = "Economy")
	bool bIsExpense = false;
};

/**
 * Player statistics for progression and telemetry
 */
USTRUCT(BlueprintType)
struct FMZPlayerStats
{
	GENERATED_BODY()

	// Distance driven in kilometers
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float TotalDistanceDriven = 0.0f;

	// Total playtime in seconds
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float TotalPlayTime = 0.0f;

	// Number of activities completed
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalActivitiesCompleted = 0;

	// Number of activities failed
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalActivitiesFailed = 0;

	// Total crash events
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalCrashes = 0;

	// Highest speed ever achieved (km/h)
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float TopSpeedEverKmh = 0.0f;

	// Longest continuous drift in seconds (placeholder)
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float LongestDrift = 0.0f;

	// Distance driven per vehicle (VehicleID -> Distance)
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	TMap<FName, float> VehicleDistances;

	// Usage count per vehicle (VehicleID -> Count)
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	TMap<FName, int32> VehicleUsageCount;

	// Driving skill rating (0-100)
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float DrivingSkill = 50.0f;
};
