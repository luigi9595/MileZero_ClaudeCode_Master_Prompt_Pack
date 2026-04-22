#include "MZNotificationWidget.h"
#include "MileZero.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Kismet/GameplayStatics.h"

void SMZNotificationWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		.Padding(FMargin(20.0f))
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		[
			SAssignNew(NotificationList, SVerticalBox)
		]
	];
}

void SMZNotificationWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	UMZNotificationManager* NotifMgr = GetNotificationManager();
	if (!NotifMgr || !NotificationList.IsValid()) return;

	// Tick the manager to update notification lifetimes
	NotifMgr->TickNotifications(InDeltaTime);

	// Rebuild the notification list
	NotificationList->ClearChildren();

	const TArray<FMZNotification>& ActiveNotifs = NotifMgr->GetActiveNotifications();
	for (const FMZNotification& Notif : ActiveNotifs)
	{
		NotificationList->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			MakeNotificationWidget(Notif)
		];
	}
}

UMZNotificationManager* SMZNotificationWidget::GetNotificationManager()
{
	if (CachedNotificationManager.IsValid())
	{
		return CachedNotificationManager.Get();
	}

	UWorld* World = GEngine->GetWorldContexts().Num() > 0
		? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World) return nullptr;

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
	{
		if (UMZNotificationManager* NotifMgr = GI->GetSubsystem<UMZNotificationManager>())
		{
			CachedNotificationManager = NotifMgr;
			return NotifMgr;
		}
	}

	return nullptr;
}

TSharedRef<SWidget> SMZNotificationWidget::MakeNotificationWidget(const FMZNotification& Notification)
{
	float Alpha = GetNotificationAlpha(Notification);

	// Get icon character based on type
	FString IconStr = TEXT("");
	switch (Notification.Type)
	{
		case EMZNotificationType::Success:
			IconStr = TEXT("✓");
			break;
		case EMZNotificationType::Warning:
			IconStr = TEXT("⚠");
			break;
		case EMZNotificationType::Money:
			IconStr = TEXT("$");
			break;
		case EMZNotificationType::Activity:
			IconStr = TEXT("◆");
			break;
		case EMZNotificationType::Zone:
			IconStr = TEXT("◈");
			break;
		default:
			IconStr = TEXT("•");
			break;
	}

	return SNew(SBox)
		.Padding(FMargin(12.0f, 8.0f))
		.WidthOverride(300.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				// Colored left border
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.BorderBackgroundColor(FLinearColor(Notification.Color.R, Notification.Color.G, Notification.Color.B, Alpha))
				.Padding(0.0f)
				[
					SNew(SBox)
					.WidthOverride(3.0f)
					.HeightOverride(30.0f)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(4.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
				.Text(FText::FromString(IconStr))
				.ColorAndOpacity(FLinearColor(Notification.Color.R, Notification.Color.G, Notification.Color.B, Alpha))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
				.Text(Notification.Message)
				.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f, Alpha))
				.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
			]
		];
}

float SMZNotificationWidget::GetNotificationAlpha(const FMZNotification& Notification) const
{
	// Fade out in the last 0.5 seconds
	float TimeRemaining = Notification.Duration - Notification.ElapsedTime;
	if (TimeRemaining <= 0.5f)
	{
		return FMath::Max(0.0f, TimeRemaining / 0.5f);
	}
	return 1.0f;
}
