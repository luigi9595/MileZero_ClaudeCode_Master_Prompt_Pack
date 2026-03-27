#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "MZWorldZoneTypes.h"
#include "MZWorldManager.generated.h"

class AMZWorldZone;
class AMZSpawnPoint;
class APawn;

/**
 * Delegate signature: fired when the player moves between zones.
 * Called with the previous zone and the new zone.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMZZoneChangeDelegate, AMZWorldZone*, PreviousZone, AMZWorldZone*, NewZone);

/**
 * UMZWorldManager: UWorldSubsystem that manages all zones and spawn points in the world.
 *
 * Responsibilities:
 * - Discovery and caching of all zone and spawn point actors at BeginPlay
 * - Tracking which zone the player is currently in
 * - Providing query API for zone/spawn point lookups
 * - Broadcasting zone change events
 * - Automatic zone detection via tick overlap checks
 *
 * Placed in the level as a UWorldSubsystem; no actor placement needed.
 */
UCLASS()
class MILEZERO_API UMZWorldManager : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMZWorldManager, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual bool IsTickableInEditor() const override { return false; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	// --- Zone Management ---

	/** All zones in the world, populated at BeginPlay */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|World")
	TArray<TObjectPtr<AMZWorldZone>> AllZones;

	/** All spawn points in the world, populated at BeginPlay */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|World")
	TArray<TObjectPtr<AMZSpawnPoint>> AllSpawnPoints;

	/** Current zone the player is in (may be null if player is between zones) */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|World")
	TObjectPtr<AMZWorldZone> CurrentZone;

	// --- Delegates ---

	/** Broadcast when player moves between zones */
	UPROPERTY(BlueprintAssignable, Category = "MZ|World")
	FMZZoneChangeDelegate OnZoneChanged;

	// --- Zone Queries ---

	/**
	 * Get the zone the player is currently in.
	 * @return Current zone, or null if player is between zones
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|World")
	AMZWorldZone* GetCurrentZone() const { return CurrentZone; }

	/**
	 * Get the nearest spawn point to the given location.
	 * Searches across all spawn points in the world.
	 * @param Location Reference location
	 * @return Nearest spawn point, or null if no spawn points exist
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|World")
	AMZSpawnPoint* GetNearestSpawnPoint(const FVector& Location) const;

	/**
	 * Get the default spawn point (usually the starting location).
	 * @return Default spawn point (bIsDefaultSpawn == true), or null if none set
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|World")
	AMZSpawnPoint* GetDefaultSpawnPoint() const;

	/**
	 * Get a zone by its ID.
	 * @param ZoneID Name identifier of the zone
	 * @return Zone with the given ID, or null if not found
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|World")
	AMZWorldZone* GetZoneByID(const FName& ZoneID) const;

	/**
	 * Get all zones of a specific type.
	 * @param ZoneType Type to filter by
	 * @return Array of zones of the given type (may be empty)
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|World")
	void GetAllZonesOfType(EMZZoneType ZoneType, TArray<AMZWorldZone*>& OutZones) const;

	/**
	 * Get a random spawn point from a specific zone.
	 * @param ZoneID Zone to get spawn point from
	 * @return Random spawn point from that zone, or null if zone not found or has no spawns
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|World")
	AMZSpawnPoint* GetRandomSpawnPointInZone(const FName& ZoneID) const;

	/**
	 * Get all spawn points in a specific zone.
	 * @param ZoneID Zone to query
	 * @return Array of spawn points in that zone (may be empty)
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|World")
	void GetSpawnPointsInZone(const FName& ZoneID, TArray<AMZSpawnPoint*>& OutSpawnPoints) const;

	/**
	 * Get world state info as a formatted string (for debug/telemetry).
	 * @return Current zone, zone count, spawn point count
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|World")
	FString GetWorldManagerInfo() const;

protected:
	/**
	 * Discover all zones and spawn points in the world.
	 * Called during Initialize.
	 */
	void DiscoverZonesAndSpawnPoints();

	/**
	 * Update the current zone based on player location.
	 * Called every tick; broadcasts OnZoneChanged if the zone has changed.
	 */
	void UpdatePlayerZone();

	/** Cached player pawn for zone tracking (updated each tick) */
	TWeakObjectPtr<APawn> CachedPlayerPawn;

	/** Previous zone for change detection */
	TObjectPtr<AMZWorldZone> PreviousZone;
};
