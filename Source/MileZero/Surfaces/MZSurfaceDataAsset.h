#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MZSurfaceDataAsset.generated.h"

UCLASS(BlueprintType)
class MILEZERO_API UMZSurfaceDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName SurfaceID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	// Grip multiplier relative to dry asphalt (1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float GripMultiplier = 1.0f;

	// Rolling resistance multiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float RollingResistance = 1.0f;

	// Surface roughness — affects suspension jitter
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Roughness = 0.0f;

	// Linked UE physical material (set in editor)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	TSoftObjectPtr<UPhysicalMaterial> PhysicalMaterial;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MZSurface", SurfaceID);
	}
};
