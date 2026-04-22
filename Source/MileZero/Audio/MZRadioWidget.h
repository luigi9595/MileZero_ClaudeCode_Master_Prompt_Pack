#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "MZRadioTypes.h"

class STextBlock;

/**
 * Slate widget for radio station display.
 * Shows station name, genre, and current track.
 * Auto-hides after 3 seconds.
 * Appears at top-center of screen.
 */
class MILEZERO_API SMZRadioWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZRadioWidget)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

	/**
	 * Display a station change notification
	 */
	void ShowStation(const FMZRadioStation& Station);

	/**
	 * Force hide the widget
	 */
	void Hide();

protected:
	TSharedPtr<STextBlock> StationNameText;
	TSharedPtr<STextBlock> GenreText;
	TSharedPtr<STextBlock> TrackText;

	float DisplayStartTime;
	float DisplayDuration;
	float CurrentOpacity;

	bool bIsVisible;
};
