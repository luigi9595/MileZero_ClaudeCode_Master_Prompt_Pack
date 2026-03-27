#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "MZActivityDataAsset.h"
#include "MZActivityHUDData.h"
#include "Damage/MZDamageTypes.h"
#include "MZActivityManager.generated.h"

class AMZCheckpointActor;
class AMZDeliveryPickup;

UENUM(BlueprintType)
enum class EMZActivityState : uint8
{
	Inactive = 0,
	Starting = 1,
	Active = 2,
	Completed = 3,
	Failed = 4
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActivityStateChanged, EMZActivityState, NewState, FText, Message, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivityProgressUpdated, FMZActivityHUDData, HUDData);

/**
 * Manages activity lifecycle, state, and progression.
 * Tracks checkpoint progression, delivery completion, time limits, and damage.
 */
UCLASS()
class MILEZERO_API UMZActivityManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Tick to update activity state
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMZActivityManager, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual bool IsTickableInEditor() const override { return false; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

protected:
	// Current activity being run
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	TObjectPtr<UMZActivityDataAsset> CurrentActivity;

	// Current state
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	EMZActivityState ActivityState = EMZActivityState::Inactive;

	// Elapsed time in seconds
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	float ElapsedTime = 0.0f;

	// Current checkpoint index (0-based)
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	int32 CurrentCheckpointIndex = -1;

	// Accumulated damage for damage survival
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	float AccumulatedDamage = 0.0f;

	// Checkpoint actors for current activity
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	TArray<TObjectPtr<AMZCheckpointActor>> ActiveCheckpoints;

	// Delivery points for current activity
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	TArray<TObjectPtr<AMZDeliveryPickup>> ActiveDeliveryPoints;

	// Best times for each activity
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	TMap<FName, float> BestTimes;

	// Has cargo been picked up (for delivery runs)
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	bool bCargoPickedUp = false;

	// Number of traffic vehicles passed (for traffic weave)
	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	int32 TrafficVehiclesPassed = 0;

	// Whether we're currently ticking
	bool bIsTickEnabled = false;

public:
	// ==== Activity Management ====

	/**
	 * Start a new activity
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void StartActivity(UMZActivityDataAsset* Activity);

	/**
	 * Abandon the current activity
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void AbandonActivity();

	/**
	 * Mark activity as complete (victory condition reached)
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void CompleteActivity();

	/**
	 * Mark activity as failed with a reason
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void FailActivity(const FString& Reason);

	// ==== Checkpoint Handling ====

	/**
	 * Called when a checkpoint is reached
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void OnCheckpointReached(int32 CheckpointIndex, bool bIsFinish);

	// ==== Delivery Handling ====

	/**
	 * Called when cargo is picked up
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void OnDeliveryPickedUp(FName DeliveryID, bool bPickedUp);

	/**
	 * Called when cargo is dropped off
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void OnDeliveryDroppedOff(FName DeliveryID);

	// ==== Traffic Weave Tracking ====

	/**
	 * Increment the traffic vehicle pass counter
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void IncrementTrafficVehiclesPassed();

	// ==== Damage Tracking ====

	/**
	 * Update accumulated damage (for damage survival activities)
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void UpdateAccumulatedDamage(float DamageAmount);

	/**
	 * Set damage from a FMZDamageState (sums all damage types)
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	void SetDamageFromState(const FMZDamageState& DamageState);

	// ==== State Queries ====

	UFUNCTION(BlueprintCallable, Category = "Activity")
	EMZActivityState GetActivityState() const { return ActivityState; }

	UFUNCTION(BlueprintCallable, Category = "Activity")
	UMZActivityDataAsset* GetCurrentActivity() const { return CurrentActivity; }

	UFUNCTION(BlueprintCallable, Category = "Activity")
	float GetElapsedTime() const { return ElapsedTime; }

	UFUNCTION(BlueprintCallable, Category = "Activity")
	int32 GetCurrentCheckpointIndex() const { return CurrentCheckpointIndex; }

	UFUNCTION(BlueprintCallable, Category = "Activity")
	float GetAccumulatedDamage() const { return AccumulatedDamage; }

	UFUNCTION(BlueprintCallable, Category = "Activity")
	int32 GetTrafficVehiclesPassed() const { return TrafficVehiclesPassed; }

	UFUNCTION(BlueprintCallable, Category = "Activity")
	bool GetCargoPickedUp() const { return bCargoPickedUp; }

	/**
	 * Get best recorded time for an activity
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	float GetBestTime(FName ActivityID) const;

	/**
	 * Get formatted objective text for current activity
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	FText GetCurrentObjectiveText() const;

	/**
	 * Get HUD data for widget binding
	 */
	UFUNCTION(BlueprintCallable, Category = "Activity")
	FMZActivityHUDData GetHUDData() const;

	// ==== Delegates ====

	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnActivityStateChanged OnActivityStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnActivityProgressUpdated OnActivityProgressUpdated;

protected:
	/**
	 * Tick the activity (called from game mode tick)
	 */
	void TickActivity(float DeltaTime);

	/**
	 * Setup checkpoints for checkpoint sprint
	 */
	void SetupCheckpointSprint();

	/**
	 * Setup delivery points for delivery run
	 */
	void SetupDeliveryRun();

	/**
	 * Setup traffic weave (no actors needed, just tracking)
	 */
	void SetupTrafficWeave();

	/**
	 * Setup damage survival (no actors needed, just damage tracking)
	 */
	void SetupDamageSurvival();

	/**
	 * Check if current objective is complete
	 */
	bool CheckActivityCompletion();

	/**
	 * Check if activity has failed (timeout, too much damage, etc)
	 */
	bool CheckActivityFailure();

	/**
	 * Clear all activity state and actors
	 */
	void ClearActivityState();

	/**
	 * Broadcast progress update
	 */
	void BroadcastProgressUpdate();

	friend class AMZGameMode; // Allow game mode to call tick
};
