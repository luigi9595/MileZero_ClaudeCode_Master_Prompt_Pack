#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MZSurfaceContactComponent.generated.h"

class UMZSurfaceDataAsset;

UCLASS(ClassGroup = (MileZero), meta = (BlueprintSpawnableComponent))
class MILEZERO_API UMZSurfaceContactComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMZSurfaceContactComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Current dominant surface under the vehicle
	UFUNCTION(BlueprintPure, Category = "MZ|Surface")
	const UMZSurfaceDataAsset* GetCurrentSurface() const { return CurrentSurface; }

	UFUNCTION(BlueprintPure, Category = "MZ|Surface")
	float GetCurrentGripMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "MZ|Surface")
	FName GetCurrentSurfaceID() const;

protected:
	virtual void BeginPlay() override;

	// Surface profile lookup table — maps physical material name to surface data asset
	UPROPERTY(EditAnywhere, Category = "MZ|Surface")
	TMap<FName, TObjectPtr<UMZSurfaceDataAsset>> SurfaceLookup;

	// Fallback surface for unknown materials
	UPROPERTY(EditAnywhere, Category = "MZ|Surface")
	TObjectPtr<UMZSurfaceDataAsset> DefaultSurface;

private:
	void DetectSurface();

	UPROPERTY()
	TObjectPtr<const UMZSurfaceDataAsset> CurrentSurface;
};
