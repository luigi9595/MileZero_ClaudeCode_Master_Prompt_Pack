#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UMZPlayerStatsSubsystem;

/**
 * Slate UI widget for displaying player statistics.
 * Shown via console command or pause menu toggle.
 * Displays: distance, playtime, activities, crashes, top speed, skill level, economy summary.
 */
class MILEZERO_API SMZStatsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZStatsWidget)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/**
	 * Update stats display from subsystem
	 */
	void RefreshStats();

	/**
	 * Close the stats widget
	 */
	void CloseStats();

protected:
	// Stats subsystem reference
	TWeakObjectPtr<UMZPlayerStatsSubsystem> StatsSubsystem;

	/**
	 * Get formatted distance text
	 */
	FText GetDistanceText() const;

	/**
	 * Get formatted playtime text
	 */
	FText GetPlaytimeText() const;

	/**
	 * Get activities completed text
	 */
	FText GetActivitiesText() const;

	/**
	 * Get total crashes text
	 */
	FText GetCrashesText() const;

	/**
	 * Get top speed text
	 */
	FText GetTopSpeedText() const;

	/**
	 * Get skill rating text
	 */
	FText GetSkillText() const;

	/**
	 * Get economy summary text (earned/spent this session)
	 */
	FText GetEconomyText() const;

	/**
	 * Build a stats row widget
	 */
	TSharedRef<SWidget> MakeStatRow(const FString& Label, const FText& Value);

	/**
	 * Handle close button
	 */
	FReply OnCloseClicked();
};
