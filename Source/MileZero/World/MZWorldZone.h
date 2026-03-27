#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZWorldZoneTypes.h"
#include "MZWorldZone.generated.h"

class UBoxComponent;
class APawn;

/**
 * Delegate signature: fired when vehicle enters/exits a zone.
 * Called with the zone and the vehicle pawn.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMZZoneVehicleDelegate, AMZWorldZone*, Zone, APawn*, Vehicle);

/**
 * AMZWorldZone: Defines a rectangular zone in the world.
 *
 * Zones are used for:
 * - Speed limit enforcement
 * - Surface type determination
 * - Spawn point management
 * - Traffic behavior customization
 * - Player zone tracking
 *
 * Each zone has a box-shaped trigger that detects vehicles entering/exiting.
 */
UCLASS()
class MILEZERO_API AMZWorldZone : public AActor
{
	GENERATED_BODY()

public:
	AMZWorldZone();

	virtual void BeginPlay() override;

	// --- Zone Identity ---

	/** Unique identifier for this zone (used for lookups) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Zone")
	FName ZoneID = NAME_None;

	/** Human-readable name displayed in UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Zone")
	FText ZoneName = FText::FromString(TEXT("Unnamed Zone"));

	/** Type of zone (City, Industrial, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Zone")
	EMZZoneType ZoneType = EMZZoneType::City;

	// --- Bounds and Detection ---

	/** Box component that triggers zone entry/exit */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Zone")
	TObjectPtr<UBoxComponent> ZoneBounds;

	// --- Driving Rules ---

	/** Default speed limit for this zone (km/h, converted to UU/s internally) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Zone", meta = (ClampMin = "0.0", ClampMax = "300.0"))
	float DefaultSpeedLimit = 50.0f;

	/** Default surface type for this zone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Zone")
	FName DefaultSurfaceType = TEXT("DryAsphalt");

	// --- Spawn Points ---

	/** Recovery/spawn points within this zone (set by designer or MZWorldManager) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Zone")
	TArray<FVector> SpawnPoints;

	// --- Zone Connectivity ---

	/** Adjacent zones that this zone connects to (for AI traffic pathfinding) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Zone")
	TArray<TObjectPtr<AMZWorldZone>> ConnectedZones;

	// --- Delegates ---

	/** Broadcast when a vehicle enters this zone */
	UPROPERTY(BlueprintAssignable, Category = "MZ|Zone")
	FMZZoneVehicleDelegate OnVehicleEntered;

	/** Broadcast when a vehicle exits this zone */
	UPROPERTY(BlueprintAssignable, Category = "MZ|Zone")
	FMZZoneVehicleDelegate OnVehicleExited;

	// --- API ---

	/**
	 * Check if a location is inside this zone.
	 * @param Location World location to test
	 * @return true if the location is inside the zone bounds
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Zone")
	bool IsLocationInZone(const FVector& Location) const;

	/**
	 * Get the nearest spawn point in this zone to the given location.
	 * @param Location Reference location
	 * @return Nearest spawn point, or zero vector if no points exist
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Zone")
	FVector GetNearestSpawnPoint(const FVector& Location) const;

	/**
	 * Get a random spawn point from this zone.
	 * @return Random spawn point, or zero vector if no points exist
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Zone")
	FVector GetRandomSpawnPoint() const;

	/**
	 * Add a connected zone for pathfinding.
	 * @param OtherZone Zone to connect to
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Zone")
	void AddConnectedZone(AMZWorldZone* OtherZone);

	/**
	 * Get zone info as a formatted string (for debug/telemetry).
	 * @return Zone name, type, speed limit, surface type
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Zone")
	FString GetZoneInfo() const;

protected:
	/**
	 * Called when an actor enters the zone bounds.
	 * Filters for vehicle pawns and broadcasts OnVehicleEntered.
	 */
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                        bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Called when an actor exits the zone bounds.
	 * Filters for vehicle pawns and broadcasts OnVehicleExited.
	 */
	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
