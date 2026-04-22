#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "MZPedestrianSubsystem.generated.h"

class AMZPedestrianActor;
class AMZVehiclePawn;
class UMZWorldManager;

/**
 * World subsystem managing pedestrian spawning and lifecycle.
 * Spawns pedestrians around the player in appropriate zones (City, Industrial).
 * Despawns those that venture too far.
 */
UCLASS()
class MILEZERO_API UMZPedestrianSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMZPedestrianSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual bool IsTickableInEditor() const override { return false; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

	// --- Configuration ---

	/** Maximum number of active pedestrians */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	int32 MaxPedestrians = 30;

	/** Radius around player to spawn pedestrians (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	float SpawnRadius = 8000.0f;

	/** Radius beyond which pedestrians are despawned (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	float DespawnRadius = 12000.0f;

	/** Minimum distance pedestrians spawn from player (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	float MinSpawnDistance = 1500.0f;

	/** Pedestrian actor class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	TSubclassOf<AMZPedestrianActor> PedestrianActorClass;

	/** Tick interval for spawn/despawn management (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians", meta = (ClampMin = "0.5", ClampMax = "5.0"))
	float ManagementTickInterval = 2.0f;

	/** Density multiplier for each zone (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	float CityDensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	float IndustrialDensity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Pedestrians")
	float RuralDensity = 0.25f;

	// --- State ---

	/** Currently active pedestrians */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Pedestrians")
	TArray<TObjectPtr<AMZPedestrianActor>> ActivePedestrians;

	// --- API ---

	/**
	 * Get number of active pedestrians.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Pedestrians")
	int32 GetActivePedestrianCount() const { return ActivePedestrians.Num(); }

	/**
	 * Notify subsystem that a pedestrian was hit by a vehicle.
	 * Pedestrian enters fleeing state or is removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Pedestrians")
	void OnPedestrianHitByVehicle(AMZPedestrianActor* Pedestrian, FVector ImpactLocation);

protected:
	// --- Internal State ---

	float ManagementTickCounter;
	TObjectPtr<UMZWorldManager> WorldManager;

	// --- Behavior ---

	void SpawnPedestriansAroundPlayer();
	void DespawnFarPedestrians();
	int32 GetZoneDensityLimit(FName ZoneID) const;
	int32 GetPedestrianCountInZone(FName ZoneID) const;
	AMZPedestrianActor* SpawnPedestrianAtLocation(FVector Location);
};
