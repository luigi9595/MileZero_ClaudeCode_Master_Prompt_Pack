#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class AMZVehiclePawn;
class AMZWorldZone;
class UMZWorldManager;

/**
 * SMZMinimapWidget: Slate widget that renders a radar-style minimap.
 * Positioned in bottom-left corner, shows player position, nearby markers, and zone name.
 * Uses FSlateDrawElement for primitive rendering (boxes, lines, circles).
 */
class MILEZERO_API SMZMinimapWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZMinimapWidget)
		: _RadiusUnits(5000.0f)
	{}
		SLATE_ARGUMENT(float, RadiusUnits)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

private:
	// Minimap settings
	float RadiusUnits = 5000.0f;
	float MinimapSize = 200.0f; // 200x200 pixels

	/**
	 * Get the player vehicle (caches result)
	 */
	AMZVehiclePawn* GetPlayerVehicle() const;

	/**
	 * Get the world manager
	 */
	UMZWorldManager* GetWorldManager() const;

	/**
	 * Convert world position to minimap screen coordinates.
	 * PlayerPos is the center (MapCenter), heading determines rotation.
	 */
	FVector2D WorldToMinimapPos(const FVector& WorldPos, const FVector& PlayerPos, float PlayerHeading) const;

	/**
	 * Draw the minimap background circle (dark semi-transparent)
	 */
	void DrawMinimapBackground(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter) const;

	/**
	 * Draw the player arrow at the center (always pointing up = forward)
	 */
	void DrawPlayerArrow(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter) const;

	/**
	 * Draw nearby traffic vehicles as red dots
	 */
	void DrawTrafficVehicles(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const;

	/**
	 * Draw activity start points as yellow dots
	 */
	void DrawActivityMarkers(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const;

	/**
	 * Draw garage locations as blue dots
	 */
	void DrawGarageMarkers(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const;

	/**
	 * Draw zone boundaries (faint lines)
	 */
	void DrawZoneBoundaries(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter, const FVector& PlayerPos, float PlayerHeading) const;

	/**
	 * Draw zone name text at the top
	 */
	void DrawZoneText(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter, const AMZWorldZone* CurrentZone) const;

	/**
	 * Draw north indicator
	 */
	void DrawNorthIndicator(FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FVector2D& MapCenter, float PlayerHeading) const;

	mutable TWeakObjectPtr<AMZVehiclePawn> CachedVehicle;
	mutable TWeakObjectPtr<UMZWorldManager> CachedWorldManager;
};
