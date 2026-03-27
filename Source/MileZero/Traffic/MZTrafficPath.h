#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZTrafficPath.generated.h"

class USplineComponent;

/**
 * Defines a traffic route/lane using a spline.
 * Traffic vehicles follow splines to move through the world.
 * Can be connected to other paths for continuous routes.
 * Assign a spline mesh to visualize in the editor (optional).
 */
UCLASS()
class MILEZERO_API AMZTrafficPath : public AActor
{
	GENERATED_BODY()

public:
	AMZTrafficPath();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Components ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	TObjectPtr<USplineComponent> PathSpline;

	// --- Configuration ---

	/** Speed limit for this path (km/h) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	float SpeedLimit = 50.0f;

	/** If true, traffic only flows in one direction along the spline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	bool bIsOneWay = true;

	/** Other traffic paths this path connects to (for continuous routing) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	TArray<TObjectPtr<AMZTrafficPath>> ConnectedPaths;

	// --- API ---

	/**
	 * Get the position at a specific distance along the spline.
	 * Distance is in Unreal units.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	FVector GetLocationAtDistance(float Distance) const;

	/**
	 * Get the forward direction at a specific distance along the spline.
	 * Normalized and tangent to the spline.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	FVector GetDirectionAtDistance(float Distance) const;

	/**
	 * Get the closest point on this path to a given world location.
	 * Returns the distance along the spline.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	float GetClosestDistanceOnPath(FVector WorldLocation) const;

	/**
	 * Get the closest world location on this path to a given point.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	FVector GetClosestLocationOnPath(FVector WorldLocation) const;

	/**
	 * Get the total length of the spline.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	float GetPathLength() const;

	/**
	 * Pick a random starting location on this path.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Traffic")
	FVector GetRandomStartLocation() const;

	/**
	 * Validate the path (checks if spline is valid and has enough points).
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	bool IsPathValid() const;
};
