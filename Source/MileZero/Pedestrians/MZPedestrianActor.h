#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZPedestrianTypes.h"
#include "MZPedestrianActor.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;

/**
 * Lightweight pedestrian NPC. Uses AActor (not ACharacter) for minimal overhead.
 * Simple tick-based movement with idle/walking/running/fleeing states.
 * When hit by vehicle: enters Fleeing state and avoids danger.
 */
UCLASS()
class MILEZERO_API AMZPedestrianActor : public AActor
{
	GENERATED_BODY()

public:
	AMZPedestrianActor();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	// --- Components ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Pedestrian")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Pedestrian")
	TObjectPtr<USkeletalMeshComponent> PedestrianMesh;

	// --- Tuning ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrian")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrian")
	float RunSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrian")
	float ObstacleCheckDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrian")
	float FleeingStateTimeout = 10.0f;

	// --- State ---

	UPROPERTY(BlueprintReadOnly, Category = "MZ|Pedestrian")
	EMZPedestrianState CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "MZ|Pedestrian")
	FVector TargetLocation;

	UPROPERTY(BlueprintReadOnly, Category = "MZ|Pedestrian")
	bool bIsRunning;

	// --- API ---

	/**
	 * Triggers fleeing behavior away from a danger source (vehicle).
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Pedestrian")
	void FleeFromLocation(FVector DangerSource);

	/**
	 * Called when pedestrian is hit by a vehicle.
	 * Initiates fleeing or despawns and respawns elsewhere.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Pedestrian")
	void OnHitByVehicle(FVector ImpactLocation);

protected:
	// --- Internal State ---

	float DistanceToTarget;
	float FleeingTimer;
	bool bIsFleeing;
	FVector FleeDirection;

	// --- Behavior ---

	void UpdateMovement(float DeltaTime);
	void PickNewTargetLocation();
	void CheckObstaclesAndAvoid();
	void UpdateState();
	void FaceMovementDirection();
};
