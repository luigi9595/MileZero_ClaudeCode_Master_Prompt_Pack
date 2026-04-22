#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZGarageActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AMZVehiclePawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVehicleGarageStateChanged, bool, bEntered);

/**
 * Garage actor placed in world for vehicle repair and cosmetic customization.
 * Handles collision detection, cost calculation, and vehicle restoration.
 */
UCLASS()
class MILEZERO_API AMZGarageActor : public AActor
{
	GENERATED_BODY()

public:
	AMZGarageActor();

	virtual void BeginPlay() override;

	/**
	 * Get the unique identifier for this garage
	 */
	UFUNCTION(BlueprintCallable, Category = "Garage")
	FName GetGarageID() const { return GarageID; }

	/**
	 * Get the display name for this garage
	 */
	UFUNCTION(BlueprintCallable, Category = "Garage")
	FText GetGarageName() const { return GarageName; }

	/**
	 * Calculate repair cost for a vehicle based on current damage
	 */
	UFUNCTION(BlueprintCallable, Category = "Garage|Repair")
	float GetRepairCost(AMZVehiclePawn* Vehicle) const;

	/**
	 * Check if player can afford to repair this vehicle
	 */
	UFUNCTION(BlueprintCallable, Category = "Garage|Repair")
	bool CanAffordRepair(AMZVehiclePawn* Vehicle) const;

	/**
	 * Repair a vehicle if player has funds
	 * @return true if repair succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Garage|Repair")
	bool RepairVehicle(AMZVehiclePawn* Vehicle);

	/**
	 * Paint vehicle with selected color
	 */
	UFUNCTION(BlueprintCallable, Category = "Garage|Cosmetics")
	bool PaintVehicle(AMZVehiclePawn* Vehicle, FLinearColor Color);

	/**
	 * Get available paint colors
	 */
	UFUNCTION(BlueprintCallable, Category = "Garage|Cosmetics")
	const TArray<FLinearColor>& GetAvailableColors() const { return AvailableColors; }

	/**
	 * Delegate fired when vehicle enters/exits garage zone
	 */
	UPROPERTY(BlueprintAssignable, Category = "Garage")
	FOnVehicleGarageStateChanged OnVehicleGarageStateChanged;

protected:
	// Garage identifier (must be unique)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garage")
	FName GarageID = FName(TEXT("MainGarage"));

	// Display name shown in UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garage")
	FText GarageName = FText::FromString(TEXT("Vehicle Garage"));

	// Trigger volume for garage detection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Garage")
	TObjectPtr<UBoxComponent> GarageZone;

	// Optional visual representation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Garage")
	TObjectPtr<UStaticMeshComponent> GarageMesh;

	// Repair cost per percentage point of damage
	// At 100% damage, cost = 50 * 100 = $5000
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garage|Costs", meta = (ClampMin = "1.0"))
	float RepairCostPerPercent = 50.0f;

	// Paint job cost (flat fee per color change)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garage|Costs", meta = (ClampMin = "0.0"))
	float PaintCostPerColor = 200.0f;

	// Available paint colors for customization
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garage|Cosmetics")
	TArray<FLinearColor> AvailableColors;

	// Currently parked vehicle (if any)
	UPROPERTY(BlueprintReadOnly, Category = "Garage")
	TObjectPtr<AMZVehiclePawn> ParkedVehicle;

	/**
	 * Called when something enters the garage zone
	 */
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	/**
	 * Called when something leaves the garage zone
	 */
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/**
	 * Initialize default paint colors if not set
	 */
	void InitializeDefaultColors();
};
