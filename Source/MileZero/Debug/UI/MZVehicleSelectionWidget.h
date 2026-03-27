#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UMZDebugSubsystem;

/**
 * SMZVehicleSelectionWidget: Slate widget for vehicle selection (debug tool).
 *
 * Displays three buttons: Coupe / Hatchback / Pickup
 * Clicking each button spawns that vehicle at the current player location.
 * Toggle visibility with "MZ.SelectVehicle" console command.
 */
class MILEZERO_API SMZVehicleSelectionWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZVehicleSelectionWidget)
	{}
		SLATE_ARGUMENT(UMZDebugSubsystem*, DebugSubsystem)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

private:
	TObjectPtr<UMZDebugSubsystem> DebugSubsystem;

	// Button callbacks
	FReply OnCoupeClicked();
	FReply OnHatchClicked();
	FReply OnPickupClicked();

	void SelectVehicle(const FString& VehicleID);
};
