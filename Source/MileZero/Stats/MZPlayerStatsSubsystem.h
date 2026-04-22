#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Economy/MZEconomyTypes.h"
#include "MZPlayerStatsSubsystem.generated.h"

class AMZVehiclePawn;

/**
 * Tracks player statistics: distance, playtime, activities, crashes, skill progression.
 * Updated every tick when a vehicle is possessed.
 * Persisted across map changes via GameInstanceSubsystem.
 */
UCLASS()
class MILEZERO_API UMZPlayerStatsSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMZPlayerStatsSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual bool IsTickableInEditor() const override { return false; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

	/**
	 * Called when vehicle is possessed (track which vehicle is in use)
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnVehiclePossessed(AMZVehiclePawn* Vehicle);

	/**
	 * Called when vehicle is unpossessed
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnVehicleUnpossessed();

	/**
	 * Called when an activity completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnActivityCompleted(const FName& ActivityID, bool bSuccess);

	/**
	 * Called when significant damage is taken (crash event)
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnCrash(float DamageApplied);

	/**
	 * Get the complete stats struct
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	const FMZPlayerStats& GetStats() const { return Stats; }

	/**
	 * Get formatted summary text of key statistics
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FText GetStatsSummary() const;

	/**
	 * Reset all stats (new game)
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ResetStats();

	/**
	 * Get top speed achieved ever
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetTopSpeedEverKmh() const { return Stats.TopSpeedEverKmh; }

	/**
	 * Get total distance driven
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetTotalDistanceDrivenKm() const { return Stats.TotalDistanceDriven; }

	/**
	 * Get driving skill rating (0-100)
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetDrivingSkill() const { return Stats.DrivingSkill; }

	/**
	 * Get total playtime in seconds
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetTotalPlayTimeSeconds() const { return Stats.TotalPlayTime; }

protected:
	// The actual statistics struct
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FMZPlayerStats Stats;

	// Currently possessed vehicle (if any)
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	TObjectPtr<AMZVehiclePawn> CurrentVehicle;

	// Currently selected vehicle ID (for per-vehicle tracking)
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FName CurrentVehicleID = FName(TEXT("None"));

	/**
	 * Update stats each frame from current vehicle
	 */
	void UpdateDistanceAndSpeed(float DeltaTime);

	/**
	 * Recalculate driving skill based on performance
	 */
	void RecalculateDrivingSkill();

	// Crash threshold: damage % that triggers a "crash" event
	static constexpr float CRASH_DAMAGE_THRESHOLD = 5.0f;

	// Last recorded speed (to avoid double-counting)
	float LastRecordedSpeedKmh = 0.0f;

	// Last time skill was recalculated
	float LastSkillRecalcTime = 0.0f;
	static constexpr float SKILL_RECALC_INTERVAL = 5.0f;
};
