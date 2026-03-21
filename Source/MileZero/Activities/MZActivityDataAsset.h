#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MZActivityDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMZActivityType : uint8
{
	CheckpointSprint,
	DeliveryRun,
	TrafficWeave,
	DamageSurvival
};

UCLASS(BlueprintType)
class MILEZERO_API UMZActivityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName ActivityID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Type")
	EMZActivityType ActivityType = EMZActivityType::CheckpointSprint;

	// Time limit in seconds (0 = no limit)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rules")
	float TimeLimitSeconds = 0.0f;

	// Max allowed damage (0 = no limit) — for damage survival
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rules")
	float MaxDamageAllowed = 0.0f;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MZActivity", ActivityID);
	}
};
