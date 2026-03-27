#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/STextBlock.h"

class AMZVehiclePawn;

class MILEZERO_API SMZTelemetryWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZTelemetryWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

private:
	TWeakObjectPtr<AMZVehiclePawn> CachedVehicle;
	TSharedPtr<STextBlock> TelemetryText;

	AMZVehiclePawn* GetPlayerVehicle();
};
