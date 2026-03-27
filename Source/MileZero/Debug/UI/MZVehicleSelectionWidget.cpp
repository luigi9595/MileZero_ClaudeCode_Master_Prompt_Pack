#include "MZVehicleSelectionWidget.h"
#include "MileZero/Debug/MZDebugSubsystem.h"
#include "SlateBasics.h"
#include "Widgets/Layout/SBox.h"

void SMZVehicleSelectionWidget::Construct(const FArguments& Args)
{
	DebugSubsystem = Args._DebugSubsystem;

	ChildSlot
	[
		SNew(SBox)
		.Padding(20.0f)
		[
			SNew(SVerticalBox)
			// Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 10.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("VEHICLE SELECTION")))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]
			// Buttons
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("COUPE")))
				.OnClicked(this, &SMZVehicleSelectionWidget::OnCoupeClicked)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("HATCHBACK")))
				.OnClicked(this, &SMZVehicleSelectionWidget::OnHatchClicked)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("PICKUP/SUV")))
				.OnClicked(this, &SMZVehicleSelectionWidget::OnPickupClicked)
			]
		]
	];
}

FReply SMZVehicleSelectionWidget::OnCoupeClicked()
{
	SelectVehicle(TEXT("Coupe"));
	return FReply::Handled();
}

FReply SMZVehicleSelectionWidget::OnHatchClicked()
{
	SelectVehicle(TEXT("Hatch"));
	return FReply::Handled();
}

FReply SMZVehicleSelectionWidget::OnPickupClicked()
{
	SelectVehicle(TEXT("Pickup"));
	return FReply::Handled();
}

void SMZVehicleSelectionWidget::SelectVehicle(const FString& VehicleID)
{
	if (DebugSubsystem)
	{
		DebugSubsystem->SpawnVehicleByID(VehicleID);
	}
}
