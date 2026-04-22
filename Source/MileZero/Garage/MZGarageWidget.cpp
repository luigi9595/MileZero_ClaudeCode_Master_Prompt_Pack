#include "MZGarageWidget.h"
#include "MZGarageActor.h"
#include "Vehicles/MZVehiclePawn.h"
#include "Economy/MZEconomySubsystem.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"

void SMZGarageWidget::Construct(const FArguments& InArgs, AMZGarageActor* InGarage, AMZVehiclePawn* InVehicle)
{
	CurrentGarage = InGarage;
	CurrentVehicle = InVehicle;

	ChildSlot
	[
		SNew(SVerticalBox)

		// Header: Garage name
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("=== GARAGE ===")))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
		]

		// Damage status
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Damage: ")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text_Lambda([this]() { return GetDamagePercentText(); })
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
				.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
			]
		]

		// Current money
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Money: ")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text_Lambda([this]() { return GetMoneyText(); })
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
				.ColorAndOpacity(FSlateColor(FLinearColor::Green))
			]
		]

		// Spacer
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		]

		// Repair section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("REPAIR")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				SNew(STextBlock)
				.Text_Lambda([this]() { return GetRepairCostText(); })
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
				.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 10.0f)
			[
				SNew(SButton)
				.IsEnabled_Lambda([this]() { return CanRepair(); })
				.OnClicked(this, &SMZGarageWidget::OnRepairClicked)
				.ButtonColorAndOpacity(FLinearColor(0.1f, 0.5f, 0.1f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Repair Vehicle")))
					.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14))
					.ColorAndOpacity(FSlateColor(FLinearColor::White))
					.Justification(ETextJustify::Center)
				]
			]
		]

		// Spacer
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		]

		// Paint section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("PAINT ($200.00)")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 10.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.0f)
				[
					MakePaintColorSwatch(FLinearColor::Red)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.0f)
				[
					MakePaintColorSwatch(FLinearColor::Blue)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.0f)
				[
					MakePaintColorSwatch(FLinearColor::Green)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.0f)
				[
					MakePaintColorSwatch(FLinearColor::White)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.0f)
				[
					MakePaintColorSwatch(FLinearColor::Black)
				]
			]
		]

		// Spacer
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		]

		// Exit button
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SButton)
			.OnClicked(this, &SMZGarageWidget::OnExitGarageClicked)
			.ButtonColorAndOpacity(FLinearColor(0.5f, 0.1f, 0.1f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Exit Garage")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
				.Justification(ETextJustify::Center)
			]
		]
	];
}

void SMZGarageWidget::UpdateGarageInfo(AMZGarageActor* InGarage, AMZVehiclePawn* InVehicle)
{
	CurrentGarage = InGarage;
	CurrentVehicle = InVehicle;
}

void SMZGarageWidget::CloseGarage()
{
	CurrentGarage = nullptr;
	CurrentVehicle = nullptr;
}

FText SMZGarageWidget::GetRepairCostText() const
{
	if (!CurrentGarage.IsValid() || !CurrentVehicle.IsValid())
	{
		return FText::FromString(TEXT("Repair: N/A"));
	}

	float Cost = CurrentGarage->GetRepairCost(CurrentVehicle.Get());
	return FText::FromString(FString::Printf(TEXT("Repair: $%.2f"), Cost));
}

bool SMZGarageWidget::CanRepair() const
{
	if (!CurrentGarage.IsValid() || !CurrentVehicle.IsValid())
	{
		return false;
	}

	return CurrentGarage->CanAffordRepair(CurrentVehicle.Get());
}

FReply SMZGarageWidget::OnRepairClicked()
{
	if (CurrentGarage.IsValid() && CurrentVehicle.IsValid())
	{
		CurrentGarage->RepairVehicle(CurrentVehicle.Get());
	}
	return FReply::Handled();
}

FReply SMZGarageWidget::OnPaintColorClicked(FLinearColor Color)
{
	if (CurrentGarage.IsValid() && CurrentVehicle.IsValid())
	{
		CurrentGarage->PaintVehicle(CurrentVehicle.Get(), Color);
	}
	return FReply::Handled();
}

FText SMZGarageWidget::GetDamagePercentText() const
{
	if (!CurrentVehicle.IsValid())
	{
		return FText::FromString(TEXT("0%"));
	}

	float DamagePercent = CurrentVehicle->GetDamagePercent();
	return FText::FromString(FString::Printf(TEXT("%.1f%%"), DamagePercent));
}

FText SMZGarageWidget::GetMoneyText() const
{
	UGameInstance* GI = nullptr;
	if (CurrentVehicle.IsValid())
	{
		GI = CurrentVehicle->GetGameInstance();
	}

	if (!GI)
	{
		return FText::FromString(TEXT("$0.00"));
	}

	UMZEconomySubsystem* EconomySubsystem = GI->GetSubsystem<UMZEconomySubsystem>();
	if (!EconomySubsystem)
	{
		return FText::FromString(TEXT("$0.00"));
	}

	return FText::FromString(FString::Printf(TEXT("$%.2f"), EconomySubsystem->GetCurrentMoney()));
}

TSharedRef<SWidget> SMZGarageWidget::MakePaintColorSwatch(FLinearColor Color)
{
	return SNew(SButton)
	.OnClicked(this, &SMZGarageWidget::OnPaintColorClicked, Color)
	.ButtonColorAndOpacity(Color)
	.ToolTip(SNew(SToolTip).Text(FText::FromString(TEXT("Click to paint"))))
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("  ")))
	];
}

FReply SMZGarageWidget::OnExitGarageClicked()
{
	CloseGarage();
	return FReply::Handled();
}
