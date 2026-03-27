#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "MZTrafficSubsystem.generated.h"

class AMZTrafficVehicle;
class AMZTrafficPath;
class AMZVehiclePawn;

/**
 * World subsystem that manages traffic AI.
 * Spawns and despawns traffic vehicles around the player.
 * Ticks to maintain desired traffic density.
 */
UCLASS()
class MILEZERO_API UMZTrafficSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMZTrafficSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual bool IsTickableInEditor() const override { return false; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

	// --- Configuration ---

	/** Maximum number of active traffic vehicles */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	int32 MaxTrafficVehicles = 20;

	/** Radius around player to spawn traffic (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	float SpawnRadius = 5000.0f;

	/** Radius beyond which traffic vehicles are despawned (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	float DespawnRadius = 8000.0f;

	/** Minimum distance traffic vehicles must be from player at spawn (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	float MinSpawnDistance = 2000.0f;

	/** Base traffic vehicle class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic")
	TSubclassOf<AMZTrafficVehicle> TrafficVehicleClass;

	/** Tick interval for spawn/despawn checks (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Traffic", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float ManagementTickInterval = 1.0f;

	// --- State ---

	/** Currently active traffic vehicles */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Traffic")
	TArray<TObjectPtr<AMZTrafficVehicle>> ActiveTraffic;

	// --- API ---

	/**
	 * Get all traffic vehicles near a location within a radius.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	int32 GetNearbyTrafficCount(FVector Location, float Radius) const;

	/**
	 * Set traffic density as a normalized value (0-1).
	 * 0 = no traffic, 1 = MaxTrafficVehicles
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Traffic")
	void SetTrafficDensity(float Density01);

	/**
	 * Get current traffic density (0-1).
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	float GetTrafficDensity() const;

	/**
	 * Spawn a traffic vehicle at a specific location on a path.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Traffic")
	AMZTrafficVehicle* SpawnTrafficVehicle(AMZTrafficPath* Path, FVector SpawnLocation);

	/**
	 * Despawn all traffic vehicles.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Traffic")
	void DespawnAllTraffic();

	/**
	 * Get list of all available traffic paths in the world.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Traffic")
	void GetAvailableTrafficPaths(TArray<AMZTrafficPath*>& OutPaths) const;

protected:
	/** Try to spawn traffic around the player */
	void SpawnTraffic();

	/** Remove traffic vehicles that are too far from player */
	void DespawnFarVehicles();

	/** Get player vehicle location */
	FVector GetPlayerLocation() const;

	/** Find a random valid traffic path */
	AMZTrafficPath* GetRandomTrafficPath();

	/** Cached player vehicle reference */
	mutable TWeakObjectPtr<AMZVehiclePawn> CachedPlayerVehicle;

	/** Time accumulator for management operations */
	float ManagementTickAccumulator = 0.0f;

	/** Cached list of traffic paths in the world */
	TArray<TWeakObjectPtr<AMZTrafficPath>> CachedTrafficPaths;

	/** Flag to rebuild cached paths list */
	bool bNeedPathsCached = true;
};
