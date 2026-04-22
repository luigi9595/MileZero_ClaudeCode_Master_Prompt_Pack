#include "MZMinimapWidget.h"
#include "MileZero.h"
#include "Vehicles/MZVehiclePawn.h"
#include "World/MZWorldManager.h"
#include "World/MZWorldZone.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SlateGameResources.h"
#include "EngineUtils.h"

void SMZMinimapWidget::Construct(const FArguments& InArgs)
{
	RadiusUnits = InArgs._RadiusUnits;

	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(MinimapSize)
		.HeightOverride(MinimapSize + 30.0f) // Extra space for zone name
	];
}

void SMZMinimapWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	// Cache refreshes happen in OnPaint
}

int32 SMZMinimapWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	int32 CurrentLayer = LayerId;

	AMZVehiclePawn* PlayerVehicle = GetPlayerVehicle();
	if (!PlayerVehicle)
	{
		return CurrentLayer;
	}

	FVector PlayerPos = PlayerVehicle->GetActorLocation();
	FVector PlayerForward = PlayerVehicle->GetActorForwardVector();
	float PlayerHeading = FMath::Atan2(PlayerForward.Y, PlayerForward.X) * 180.0f / PI;

	// Minimap center (in widget space)
	FVector2D MapCenter = AllottedGeometry.GetLocalSize() * 0.5f;
	MapCenter.Y = MinimapSize * 0.5f; // Adjust for zone name area

	// Draw elements
	DrawMinimapBackground(OutDrawElements, CurrentLayer, MapCenter);
	CurrentLayer++;

	DrawZoneBoundaries(OutDrawElements, CurrentLayer, MapCenter, PlayerPos, PlayerHeading);
	CurrentLayer++;

	DrawTrafficVehicles(OutDrawElements, CurrentLayer, MapCenter, PlayerPos, PlayerHeading);
	CurrentLayer++;

	DrawActivityMarkers(OutDrawElements, CurrentLayer, MapCenter, PlayerPos, PlayerHeading);
	CurrentLayer++;

	DrawGarageMarkers(OutDrawElements, CurrentLayer, MapCenter, PlayerPos, PlayerHeading);
	CurrentLayer++;

	DrawPlayerArrow(OutDrawElements, CurrentLayer, MapCenter);
	CurrentLayer++;

	DrawNorthIndicator(OutDrawElements, CurrentLayer, MapCenter, PlayerHeading);
	CurrentLayer++;

	AMZWorldZone* CurrentZone = nullptr;
	if (UMZWorldManager* WorldMgr = GetWorldManager())
	{
		CurrentZone = WorldMgr->GetCurrentZone();
	}
	DrawZoneText(OutDrawElements, CurrentLayer, MapCenter, CurrentZone);
	CurrentLayer++;

	return CurrentLayer;
}

AMZVehiclePawn* SMZMinimapWidget::GetPlayerVehicle() const
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

UMZWorldManager* SMZMinimapWidget::GetWorldManager() const
{
	if (CachedWorldManager.IsValid()) return CachedWorldManager.Get();

	UWorld* World = GEngine->GetWorldContexts().Num() > 0
		? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World) return nullptr;

	if (UMZWorldManager* WorldMgr = World->GetSubsystem<UMZWorldManager>())
	{
		CachedWorldManager = WorldMgr;
		return WorldMgr;
	}
	return nullptr;
}

FVector2D SMZMinimapWidget::WorldToMinimapPos(const FVector& WorldPos, const FVector& PlayerPos, float PlayerHeading) const
{
	FVector2D MapCenter(MinimapSize * 0.5f, MinimapSize * 0.5f);

	// Relative position in world space
	FVector RelPos = WorldPos - PlayerPos;

	// Rotate by -PlayerHeading so the player's forward direction points up
	float RadHeading = -PlayerHeading * PI / 180.0f;
	float CosTh = FMath::Cos(RadHeading);
	float SinTh = FMath::Sin(RadHeading);

	FVector2D RotatedPos(
		RelPos.X * CosTh - RelPos.Y * SinTh,
		RelPos.X * SinTh + RelPos.Y * CosTh
	);

	// Scale to minimap coordinates
	float PixelsPerUnit = MinimapSize * 0.5f / RadiusUnits;
	FVector2D MinimapOffset = RotatedPos * PixelsPerUnit;

	// Clamp to minimap bounds
	FVector2D Result = MapCenter + MinimapOffset;
	float MaxDist = MinimapSize * 0.5f - 2.0f;
	if (MinimapOffset.Length() > MaxDist)
	{
		MinimapOffset = MinimapOffset.GetSafeNormal() * MaxDist;
		Result = MapCenter + MinimapOffset;
	}

	return Result;
}

void SMZMinimapWidget::DrawMinimapBackground(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter) const
{
	// Semi-transparent dark circle background
	FVector2D BoxPos = MapCenter - FVector2D(MinimapSize * 0.5f);
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		FPaintGeometry(BoxPos, FVector2D(MinimapSize, MinimapSize), 1.0f),
		FCoreStyle::Get().GetBrush("WhiteSquare"),
		ESlateDrawEffect::None,
		FLinearColor(0.1f, 0.1f, 0.1f, 0.8f)
	);
}

void SMZMinimapWidget::DrawPlayerArrow(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter) const
{
	// Draw a simple upward-pointing triangle (player always at center, pointing up)
	float ArrowSize = 10.0f;

	TArray<FVector2D> ArrowPoints;
	ArrowPoints.Add(MapCenter + FVector2D(0.0f, -ArrowSize)); // Top point
	ArrowPoints.Add(MapCenter + FVector2D(-ArrowSize * 0.7f, ArrowSize * 0.5f)); // Bottom-left
	ArrowPoints.Add(MapCenter + FVector2D(ArrowSize * 0.7f, ArrowSize * 0.5f)); // Bottom-right

	TArray<FVector2f> ArrowPointsF;
	for (const FVector2D& Pt : ArrowPoints) { ArrowPointsF.Add(FVector2f(Pt)); }
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		FPaintGeometry(FVector2D::ZeroVector, FVector2D(1.0, 1.0), 1.0f),
		ArrowPointsF,
		ESlateDrawEffect::None,
		FLinearColor::Yellow,
		true,
		1.5f
	);
}

void SMZMinimapWidget::DrawTrafficVehicles(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const
{
	UWorld* World = GEngine->GetWorldContexts().Num() > 0
		? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World) return;

	// Find nearby traffic vehicles (excluding player vehicle)
	for (TActorIterator<AMZVehiclePawn> It(World); It; ++It)
	{
		AMZVehiclePawn* Vehicle = *It;
		if (!Vehicle || Vehicle == GetPlayerVehicle()) continue;

		float Dist = FVector::Dist(Vehicle->GetActorLocation(), PlayerPos);
		if (Dist > RadiusUnits) continue;

		FVector2D MinimapPos = WorldToMinimapPos(Vehicle->GetActorLocation(), PlayerPos, PlayerHeading);

		// Draw as small red dot
		FVector2D DotPos = MinimapPos - FVector2D(2.0f, 2.0f);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			FPaintGeometry(DotPos, FVector2D(4.0, 4.0), 1.0f),
			FCoreStyle::Get().GetBrush("WhiteSquare"),
			ESlateDrawEffect::None,
			FLinearColor::Red
		);
	}
}

void SMZMinimapWidget::DrawActivityMarkers(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const
{
	// TODO: Query activity manager for active activity locations
	// For now, stub: would iterate through checkpoint actors and delivery pickups
	UE_LOG(LogMileZero, Log, TEXT("DrawActivityMarkers: stub until activity markers are populated"));
}

void SMZMinimapWidget::DrawGarageMarkers(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const
{
	// TODO: Query for garage/spawn point locations
	// For now, stub: would iterate through spawn points marked as garages
	UE_LOG(LogMileZero, Log, TEXT("DrawGarageMarkers: stub until garage markers are populated"));
}

void SMZMinimapWidget::DrawZoneBoundaries(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const
{
	// TODO: Draw zone boundary lines if in range
	UE_LOG(LogMileZero, Log, TEXT("DrawZoneBoundaries: stub until zone boundaries are available"));
}

void SMZMinimapWidget::DrawZoneText(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter, const AMZWorldZone* CurrentZone) const
{
	FString ZoneNameStr = CurrentZone ? CurrentZone->ZoneName.ToString() : TEXT("Unknown Zone");

	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId,
		FPaintGeometry(FVector2D(20.0, 5.0), FVector2D(1.0, 1.0), 1.0f),
		ZoneNameStr,
		FCoreStyle::GetDefaultFontStyle("Bold", 12),
		ESlateDrawEffect::None,
		FLinearColor::White
	);
}

void SMZMinimapWidget::DrawNorthIndicator(FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FVector2D& MapCenter, float PlayerHeading) const
{
	// Draw a small "N" at the top of the minimap, rotated to show north direction
	float RadHeading = -PlayerHeading * PI / 180.0f;
	FVector2D NorthDir(FMath::Cos(RadHeading), FMath::Sin(RadHeading));
	FVector2D NorthPos = MapCenter + (NorthDir * (MinimapSize * 0.4f));

	FVector2D NorthTextPos = NorthPos - FVector2D(5.0f, 5.0f);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId,
		FPaintGeometry(NorthTextPos, FVector2D(1.0, 1.0), 1.0f),
		FString(TEXT("N")),
		FCoreStyle::GetDefaultFontStyle("Bold", 10),
		ESlateDrawEffect::None,
		FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)
	);
}
