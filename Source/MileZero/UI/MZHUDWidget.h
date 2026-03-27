#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/STextBlock.h"

class AMZVehiclePawn;

class MILEZERO_API SMZHUDWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZHUDWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

private:
	TWeakObjectPtr<AMZVehiclePawn> CachedVehicle;
	TSharedPtr<STextBlock> SpeedText;
	TSharedPtr<STextBlock> InfoText;

	AMZVehiclePawn* GetPlayerVehicle();
};
