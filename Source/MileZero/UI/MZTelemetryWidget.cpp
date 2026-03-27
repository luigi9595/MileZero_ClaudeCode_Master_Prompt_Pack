#include "MZTelemetryWidget.h"
#include "MileZero.h"
#include "Vehicles/MZVehiclePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Layout/SBox.h"

void SMZTelemetryWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		.Padding(FMargin(20.0f))
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SAssignNew(TelemetryText, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 14))
			.ColorAndOpacity(FLinearColor::Green)
			.ShadowOffset(FVector2D(1, 1))
			.ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.8f))
			.Text(FText::FromString(TEXT("MZ Telemetry: waiting...")))
		]
	];
}

void SMZTelemetryWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	AMZVehiclePawn* Vehicle = GetPlayerVehicle();
	if (!Vehicle || !TelemetryText.IsValid()) return;

	float Speed = Vehicle->GetSpeedKmh();
	float RPM = Vehicle->GetRPM();
	int32 Gear = Vehicle->GetCurrentGear();
	float Throttle = Vehicle->GetThrottleInput();
	float Brake = Vehicle->GetBrakeInput();
	float Steer = Vehicle->GetSteeringInput();
	FName Surface = Vehicle->GetCurrentSurfaceID();
	float Grip = Vehicle->GetCurrentGripMultiplier();
	float Damage = Vehicle->GetDamagePercent();
	float FPS = (InDeltaTime > 0.0f) ? (1.0f / InDeltaTime) : 0.0f;

	FString Info = FString::Printf(
		TEXT("=== MZ TELEMETRY ===\n")
		TEXT("Speed: %6.1f km/h\n")
		TEXT("RPM:   %6.0f\n")
		TEXT("Gear:  %d\n")
		TEXT("Throttle: %.2f  Brake: %.2f  Steer: %+.2f\n")
		TEXT("Surface: %s  Grip: %.2f\n")
		TEXT("Damage: %.0f%%\n")
		TEXT("FPS: %.0f"),
		Speed, RPM, Gear,
		Throttle, Brake, Steer,
		*Surface.ToString(), Grip,
		Damage,
		FPS
	);

	TelemetryText->SetText(FText::FromString(Info));

	// Color-code by grip
	FLinearColor Color = FLinearColor::Green;
	if (Grip < 0.5f) Color = FLinearColor::Red;
	else if (Grip < 0.8f) Color = FLinearColor::Yellow;
	TelemetryText->SetColorAndOpacity(Color);
}

AMZVehiclePawn* SMZTelemetryWidget::GetPlayerVehicle()
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
