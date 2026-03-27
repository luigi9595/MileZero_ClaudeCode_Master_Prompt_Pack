#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZDeliveryPickup.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeliveryInteraction, FName, DeliveryID, bool, bPickedUp);

/**
 * Delivery point actor: either a pickup location or a dropoff location.
 * Used in delivery run activities.
 */
UCLASS()
class MILEZERO_API AMZDeliveryPickup : public AActor
{
	GENERATED_BODY()

public:
	AMZDeliveryPickup();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	// Trigger zone for delivery interaction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Delivery")
	TObjectPtr<UBoxComponent> PickupZone;

	// Optional visual mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Delivery")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	// Unique identifier for this delivery point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	FName DeliveryID;

	// True if this is a pickup point, false if dropoff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	bool bIsPickup = true;

	// Radius of the pickup zone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	float PickupRadius = 500.0f;

	// Color for visualization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery|Visual")
	FLinearColor PickupColor = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery|Visual")
	FLinearColor DropoffColor = FLinearColor::Yellow;

	// Called when a pawn enters the zone
	UFUNCTION()
	void OnPickupZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	// Delegate fired when entered
	UPROPERTY(BlueprintAssignable, Category = "Delivery")
	FOnDeliveryInteraction OnDeliveryInteraction;

	// Update visual state (active/inactive)
	UFUNCTION(BlueprintCallable, Category = "Delivery")
	void SetDeliveryActive(bool bActive);

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Delivery")
	FName GetDeliveryID() const { return DeliveryID; }

	UFUNCTION(BlueprintCallable, Category = "Delivery")
	bool GetIsPickup() const { return bIsPickup; }

	UFUNCTION(BlueprintCallable, Category = "Delivery")
	float GetPickupRadius() const { return PickupRadius; }
};
