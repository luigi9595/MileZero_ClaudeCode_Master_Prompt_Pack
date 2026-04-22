#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class AMZGarageActor;
class AMZVehiclePawn;

/**
 * Slate UI widget displayed when player is inside a garage zone.
 * Shows repair cost, available paint colors, and exit button.
 */
class MILEZERO_API SMZGarageWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZGarageWidget)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, AMZGarageActor* InGarage, AMZVehiclePawn* InVehicle);

	/**
	 * Update widget with current garage and vehicle info
	 */
	void UpdateGarageInfo(AMZGarageActor* InGarage, AMZVehiclePawn* InVehicle);

	/**
	 * Close the garage widget
	 */
	void CloseGarage();

protected:
	// Garage being displayed
	TWeakObjectPtr<AMZGarageActor> CurrentGarage;

	// Vehicle currently in garage
	TWeakObjectPtr<AMZVehiclePawn> CurrentVehicle;

	/**
	 * Get repair cost text
	 */
	FText GetRepairCostText() const;

	/**
	 * Check if repair button should be enabled
	 */
	bool CanRepair() const;

	/**
	 * Handle repair button click
	 */
	FReply OnRepairClicked();

	/**
	 * Handle paint color selection
	 */
	FReply OnPaintColorClicked(FLinearColor Color);

	/**
	 * Get current damage percentage text
	 */
	FText GetDamagePercentText() const;

	/**
	 * Get current money text
	 */
	FText GetMoneyText() const;

	/**
	 * Build paint color swatch
	 */
	TSharedRef<SWidget> MakePaintColorSwatch(FLinearColor Color);

	/**
	 * Handle exit garage button
	 */
	FReply OnExitGarageClicked();
};
