#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZRouteMarker.generated.h"

class UBillboardComponent;

/**
 * AMZRouteMarker: Defines a waypoint or route segment between zones.
 *
 * Route markers are placed by designers to define recommended driving paths
 * through the world. They help with:
 * - AI traffic pathfinding
 * - Player objective waypoints
 * - Rally/checkpoint definitions
 * - Road layout visualization
 *
 * Each marker knows the zones it connects and its order in the route.
 */
UCLASS()
class MILEZERO_API AMZRouteMarker : public AActor
{
	GENERATED_BODY()

public:
	AMZRouteMarker();

	virtual void PostInitializeComponents() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// --- Route Identity ---

	/** Unique identifier for this route or waypoint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Route")
	FName RouteID = NAME_None;

	/** Order of this marker in the route (0 = start, 1 = next, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Route", meta = (ClampMin = "0"))
	int32 SequenceIndex = 0;

	// --- Zone Connectivity ---

	/** Zone this route starts from (or is contained within) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Route")
	FName FromZoneID = NAME_None;

	/** Zone this route leads to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Route")
	FName ToZoneID = NAME_None;

	// --- Editor Helpers (editor-only) ---

	/** Billboard sprite visible in editor (editor-only) */
	UPROPERTY(VisibleAnywhere, Category = "MZ|Route")
	TObjectPtr<UBillboardComponent> EditorIcon;

	// --- API ---

	/**
	 * Get route marker info as a formatted string (for debug/telemetry).
	 * @return Route ID, sequence, zones, location
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Route")
	FString GetRouteMarkerInfo() const;

	/**
	 * Get the distance to another route marker.
	 * @param OtherMarker The marker to measure distance to
	 * @return Euclidean distance in UU
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Route")
	float GetDistanceToMarker(const AMZRouteMarker* OtherMarker) const;
};
