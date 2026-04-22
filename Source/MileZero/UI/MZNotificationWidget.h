#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "MZNotificationManager.h"

/**
 * SMZNotificationWidget: Slate widget that renders active notifications.
 * Positioned in top-right corner, displays notification stack with fade-out animation.
 */
class MILEZERO_API SMZNotificationWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZNotificationWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

private:
	/**
	 * Get the notification manager subsystem
	 */
	UMZNotificationManager* GetNotificationManager();

	/**
	 * Generate a widget for a single notification
	 */
	TSharedRef<SWidget> MakeNotificationWidget(const FMZNotification& Notification);

	/**
	 * Calculate alpha (fade) for a notification based on elapsed time
	 */
	float GetNotificationAlpha(const FMZNotification& Notification) const;

	TWeakObjectPtr<UMZNotificationManager> CachedNotificationManager;
	TSharedPtr<SVerticalBox> NotificationList;
};
