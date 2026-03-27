#include "MZHUDWidget.h"
#include "MileZero.h"
#include "Vehicles/MZVehiclePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"

void SMZHUDWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		.Padding(FMargin(20.0f))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SAssignNew(SpeedText, STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 36))
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FVector2D(2, 2))
				.ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.9f))
				.Text(FText::FromString(TEXT("0 km/h")))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(0, 4, 0, 0)
			[
				SAssignNew(InfoText, STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Mono", 14))
				.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
				.ShadowOffset(FVector2D(1, 1))
				.ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.8f))
				.Text(FText::FromString(TEXT("RPM: 0 | Gear: N | Damage: 0%")))
			]
		]
	];
}

void SMZHUDWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	AMZVehiclePawn* Vehicle = GetPlayerVehicle();
	if (!Vehicle) return;

	float Speed = Vehicle->GetSpeedKmh();
	float RPM = Vehicle->GetRPM();
	int32 Gear = Vehicle->GetCurrentGear();
	float Damage = Vehicle->GetDamagePercent();

	if (SpeedText.IsValid())
	{
		SpeedText->SetText(FText::FromString(FString::Printf(TEXT("%.0f km/h"), FMath::Abs(Speed))));
	}

	if (InfoText.IsValid())
	{
		FString GearStr = (Gear == 0) ? TEXT("N") : ((Gear < 0) ? TEXT("R") : FString::Printf(TEXT("%d"), Gear));
		InfoText->SetText(FText::FromString(FString::Printf(
			TEXT("RPM: %.0f | Gear: %s | Damage: %.0f%%"),
			RPM, *GearStr, Damage
		)));
	}
}

AMZVehiclePawn* SMZHUDWidget::GetPlayerVehicle()
{
	if (CachedVehicle.IsValid()) return CachedVehicle.Get();

	UWorld* World = GEngine->GetWorldContexts().Num() > 0
		? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World) return nullptr;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (PC)
	{
		AMZVehiclePawn* V = Cast<AMZVehiclePawn>(PC->GetPawn());
		if (V) CachedVehicle = V;
		return V;
	}
	return nullptr;
}
