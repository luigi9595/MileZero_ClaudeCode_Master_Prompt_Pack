#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZVehicleRegistry.generated.h"

class UMZVehicleDataAsset;

/**
 * MZVehicleRegistry: Game Instance subsystem managing all vehicle data assets.
 *
 * Serves as the single source of truth for vehicle specifications.
 * Creates and manages the three default vehicle profiles programmatically.
 * Used by MZGameMode to spawn vehicles and by the debug subsystem for vehicle selection.
 */
UCLASS()
class MILEZERO_API UMZVehicleRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * Get a vehicle data asset by ID.
	 * @param VehicleID The vehicle identifier (e.g., "Coupe", "Hatch", "Pickup")
	 * @return Pointer to the UMZVehicleDataAsset, or nullptr if not found
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Vehicle")
	UMZVehicleDataAsset* GetVehicleData(FName VehicleID) const;

	/**
	 * Get all registered vehicle IDs.
	 * @return Array of vehicle ID names
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Vehicle")
	TArray<FName> GetAllVehicleIDs() const;

	/**
	 * Get the default vehicle ID (Hatch).
	 * @return Default vehicle ID for new games
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Vehicle")
	FName GetDefaultVehicleID() const { return FName("Hatch"); }

	/**
	 * Get a vehicle data asset by display name (useful for UI).
	 * @param DisplayName The human-readable name
	 * @return Pointer to the UMZVehicleDataAsset, or nullptr if not found
	 */
	UMZVehicleDataAsset* GetVehicleDataByDisplayName(const FText& DisplayName) const;

private:
	/**
	 * Internal map of vehicle ID -> data asset.
	 * Populated during Initialize().
	 */
	UPROPERTY()
	TMap<FName, TObjectPtr<UMZVehicleDataAsset>> VehicleMap;

	/**
	 * Create all three default vehicle profiles programmatically.
	 */
	void CreateDefaultVehicles();

	/**
	 * Create a coupe data asset (RWD, agile, 1300 kg).
	 */
	UMZVehicleDataAsset* CreateCoupeData();

	/**
	 * Create a hatchback data asset (FWD, stable, 1150 kg).
	 */
	UMZVehicleDataAsset* CreateHatchData();

	/**
	 * Create a pickup/SUV data asset (AWD, heavy, 2000 kg).
	 */
	UMZVehicleDataAsset* CreatePickupData();
};
