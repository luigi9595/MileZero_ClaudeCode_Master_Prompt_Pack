#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZCheckpointActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCheckpointReached, int32, CheckpointIndex, bool, bIsFinish);

/**
 * Checkpoint trigger actor for activities.
 * Each checkpoint is a trigger volume that notifies the activity manager when reached.
 */
UCLASS()
class MILEZERO_API AMZCheckpointActor : public AActor
{
	GENERATED_BODY()

public:
	AMZCheckpointActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	// Trigger volume for overlap detection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	TObjectPtr<UBoxComponent> TriggerVolume;

	// Optional visual mesh to show checkpoint location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	// Index in the checkpoint sequence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	int32 CheckpointIndex = 0;

	// Is this the starting checkpoint?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	bool bIsStart = false;

	// Is this the finish checkpoint?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	bool bIsFinish = false;

	// Visual color: green for next, grey for passed, red for unreachable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Visual")
	FLinearColor ColorActive = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Visual")
	FLinearColor ColorPassed = FLinearColor(0.4f, 0.4f, 0.4f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Visual")
	FLinearColor ColorUnreachable = FLinearColor::Red;

	// Current state
	UPROPERTY(BlueprintReadOnly, Category = "Checkpoint")
	bool bIsReached = false;

	UPROPERTY(BlueprintReadOnly, Category = "Checkpoint")
	bool bIsReachable = false;

	// Called when a pawn enters the trigger
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	// Delegate fired when reached
	UPROPERTY(BlueprintAssignable, Category = "Checkpoint")
	FOnCheckpointReached OnCheckpointReached;

	// Called by activity manager to update visual state
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetCheckpointState(bool bReachable, bool bReached);

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	int32 GetCheckpointIndex() const { return CheckpointIndex; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	bool GetIsStart() const { return bIsStart; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	bool GetIsFinish() const { return bIsFinish; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	bool GetIsReached() const { return bIsReached; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	bool GetIsReachable() const { return bIsReachable; }
};
