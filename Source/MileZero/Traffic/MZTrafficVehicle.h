#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MZTrafficVehicle.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMZOnPlayerCollision);

class AMZTrafficPath;
class UBoxComponent;
class UStaticMeshComponent;
class UFloatingPawnMovement;

/**
 * Simple AI traffic vehicle that follows assigned paths.
 * Uses simple movement and raycasts for obstacle avoidance.
 * NOT a full Chaos physics vehicle - designed to be cheap and scalable.
 * Vehicles are assigned paths and speeds in editor.
 */
UCLASS()
class MILEZERO_API AMZTrafficVehicle : public APawn
{
	GENERATED_BODY()

public:
	AMZTrafficVehicle();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- Components ---

	/** Collision box for the vehicle */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	TObjectPtr<UBoxComponent> CollisionBox;

	/** Visual mesh for the vehicle (assign in editor) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	TObjectPtr<UStaticMeshComponent> VehicleMesh;

	/** Simple movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	TObjectPtr<UFloatingPawnMovement> MovementComponent;

	// --- Configuration ---

	/** Current desired speed (km/h) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	float DesiredSpeed = 60.0f;

	/** The traffic path this vehicle is following */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	TObjectPtr<AMZTrafficPath> CurrentPath;

	/** How fast the vehicle accelerates/brakes (0-1 per second) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float AccelerationRate = 1.0f;

	/** How far ahead to raytrace for obstacles (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic", meta = (ClampMin = "100.0", ClampMax = "5000.0"))
	float ObstacleCheckDistance = 1500.0f;

	/** Raycast width for multi-lane obstacle detection (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float ObstacleCheckWidth = 300.0f;

	// --- State ---

	/** Current speed (km/h) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	float CurrentSpeed = 0.0f;

	/** True if vehicle is stopped (waiting at obstacle) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	bool bIsStopped = false;

	/** True if vehicle is yielding to another vehicle */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	bool bIsYielding = false;

	/** Distance traveled along current path (cm) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	float DistanceAlongPath = 0.0f;

	// --- API ---

	/**
	 * Assign a new path for this vehicle to follow.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Traffic")
	void SetPath(AMZTrafficPath* NewPath);

	/**
	 * Check if there's an obstacle ahead on the path.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	bool IsObstacleAhead() const;

	/**
	 * Get the current speed in km/h.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	float GetSpeedKmh() const { return CurrentSpeed; }

	/**
	 * Set vehicle as despawned (cleanup).
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Traffic")
	void Despawn();

	// --- Events ---

	/** Called when this vehicle is hit by the player */
	UPROPERTY(BlueprintAssignable, Category = "MZ|Traffic")
	FMZOnPlayerCollision OnPlayerCollision;

protected:
	/** Follow the assigned path */
	void FollowPath(float DeltaTime);

	/** Handle hit response */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	/** Advance to next path segment (for path continuity) */
	void AdvanceToNextPath();

	/** Last cached obstacle ahead state (for smooth transitions) */
	bool bLastObstacleState = false;
};
