#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MZDamageDataAsset.generated.h"

/**
 * Data-driven damage configuration asset
 * Controls all damage thresholds, maximum damage amounts, and recovery behavior
 */
UCLASS(BlueprintType)
class MILEZERO_API UMZDamageDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Identity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName DamageProfileID = FName("DefaultDamageProfile");

	// --- Impact Thresholds (km/h) ---
	// Damage is not applied below LightImpactSpeed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact Thresholds", meta = (ClampMin = "1.0"))
	float LightImpactSpeed = 15.0f;

	// Damage type escalates to medium severity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact Thresholds", meta = (ClampMin = "5.0"))
	float MediumImpactSpeed = 30.0f;

	// Damage type escalates to heavy severity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact Thresholds", meta = (ClampMin = "20.0"))
	float HeavyImpactSpeed = 60.0f;

	// --- Maximum Damage Values ---
	// Maximum steering pull in degrees (0-1 damage multiplied by this)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max Damage", meta = (ClampMin = "1.0", ClampMax = "45.0"))
	float MaxSteeringPull = 15.0f;

	// Maximum power loss fraction (0.5 = 50% power loss at full engine damage)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max Damage", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxPowerLoss = 0.5f;

	// Maximum drag increase (multiplier: 1.0 + this value at full aero damage)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max Damage", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxDragIncrease = 0.3f;

	// Maximum brake degradation fraction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max Damage", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxBrakeDegrade = 0.4f;

	// Maximum suspension effectiveness loss per wheel
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max Damage", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxSuspensionDegrade = 0.5f;

	// Maximum cooling system degradation (affects overheat rate)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max Damage", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxCoolingDegrade = 0.5f;

	// --- Recovery Settings ---
	// Auto-repair rate per second (0.0 = no auto-repair, 1.0 = full repair per second)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recovery", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RepairRate = 0.0f;

	// Allow full reset via ResetVehicle()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recovery")
	bool bCanReset = true;

	// Primary Asset ID support
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MZDamageProfile", DamageProfileID);
	}
};
