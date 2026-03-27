#pragma once

#include "CoreMinimal.h"
#include "MZDamageTypes.generated.h"

/**
 * Severity classification for collision impacts
 */
UENUM(BlueprintType)
enum class EMZImpactSeverity : uint8
{
	None = 0,
	Light = 1,
	Medium = 2,
	Heavy = 3
};

/**
 * Snapshot of all damage state for serialization (save/load)
 */
USTRUCT(BlueprintType)
struct FMZDamageState
{
	GENERATED_BODY()

	// Steering system damage (causes handling pull)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float SteeringDamage = 0.0f;

	// Engine power loss
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float EngineDamage = 0.0f;

	// Cooling system degradation (leads to overheating)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float CoolingDamage = 0.0f;

	// Aerodynamic damage (increases drag)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float AeroDamage = 0.0f;

	// Brake system damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float BrakeDamage = 0.0f;

	// Per-wheel suspension effectiveness (FL, FR, RL, RR)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	TArray<float> SuspensionDamage = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Per-wheel traction loss (FL, FR, RL, RR)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	TArray<float> WheelDamage = { 0.0f, 0.0f, 0.0f, 0.0f };
};
