#include "MZStatsWidget.h"
#include "MZPlayerStatsSubsystem.h"
#include "Economy/MZEconomySubsystem.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Engine/GameInstance.h"

void SMZStatsWidget::Construct(const FArguments& InArgs)
{
	// Get the stats subsystem
	UGameInstance* GI = GEngine->GetCurrentPlayWorld() ? GEngine->GetCurrentPlayWorld()->GetGameInstance() : nullptr;
	if (GI)
	{
		StatsSubsystem = GI->GetSubsystem<UMZPlayerStatsSubsystem>();
	}

	ChildSlot
	[
		SNew(SVerticalBox)

		// Header
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(15.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("=== PLAYER STATISTICS ===")))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
			.Justification(ETextJustify::Center)
		]

		// Distance Driven
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeStatRow(TEXT("Distance Driven:"), GetDistanceText())
		]

		// Total Playtime
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeStatRow(TEXT("Total Playtime:"), GetPlaytimeText())
		]

		// Activities
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeStatRow(TEXT("Activities:"), GetActivitiesText())
		]

		// Crashes
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeStatRow(TEXT("Total Crashes:"), GetCrashesText())
		]

		// Top Speed
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeStatRow(TEXT("Top Speed:"), GetTopSpeedText())
		]

		// Driving Skill
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeStatRow(TEXT("Driving Skill:"), GetSkillText())
		]

		// Spacer
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		]

		// Economy Summary
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeStatRow(TEXT("Session Economy:"), GetEconomyText())
		]

		// Spacer
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		]

		// Close button
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(15.0f)
		[
			SNew(SButton)
			.OnClicked(this, &SMZStatsWidget::OnCloseClicked)
			.ButtonColorAndOpacity(FLinearColor(0.5f, 0.1f, 0.1f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Close")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
				.Justification(ETextJustify::Center)
			]
		]
	];
}

void SMZStatsWidget::RefreshStats()
{
	// Widget is lambda-based, so just accessing properties will cause re-evaluation
	// Nothing to do here explicitly
}

void SMZStatsWidget::CloseStats()
{
	StatsSubsystem = nullptr;
}

FText SMZStatsWidget::GetDistanceText() const
{
	if (!StatsSubsystem.IsValid())
	{
		return FText::FromString(TEXT("0.0 km"));
	}

	float Distance = StatsSubsystem->GetTotalDistanceDrivenKm();
	return FText::FromString(FString::Printf(TEXT("%.1f km"), Distance));
}

FText SMZStatsWidget::GetPlaytimeText() const
{
	if (!StatsSubsystem.IsValid())
	{
		return FText::FromString(TEXT("0 min"));
	}

	float TotalSeconds = StatsSubsystem->GetTotalPlayTimeSeconds();
	int32 Minutes = static_cast<int32>(TotalSeconds) / 60;
	int32 Seconds = static_cast<int32>(TotalSeconds) % 60;

	return FText::FromString(FString::Printf(TEXT("%d:%02d"), Minutes, Seconds));
}

FText SMZStatsWidget::GetActivitiesText() const
{
	if (!StatsSubsystem.IsValid())
	{
		return FText::FromString(TEXT("0 / 0"));
	}

	const FMZPlayerStats& Stats = StatsSubsystem->GetStats();
	return FText::FromString(FString::Printf(TEXT("%d Completed / %d Failed"),
		Stats.TotalActivitiesCompleted, Stats.TotalActivitiesFailed));
}

FText SMZStatsWidget::GetCrashesText() const
{
	if (!StatsSubsystem.IsValid())
	{
		return FText::FromString(TEXT("0"));
	}

	const FMZPlayerStats& Stats = StatsSubsystem->GetStats();
	return FText::FromString(FString::Printf(TEXT("%d"), Stats.TotalCrashes));
}

FText SMZStatsWidget::GetTopSpeedText() const
{
	if (!StatsSubsystem.IsValid())
	{
		return FText::FromString(TEXT("0 km/h"));
	}

	float TopSpeed = StatsSubsystem->GetTopSpeedEverKmh();
	return FText::FromString(FString::Printf(TEXT("%.1f km/h"), TopSpeed));
}

FText SMZStatsWidget::GetSkillText() const
{
	if (!StatsSubsystem.IsValid())
	{
		return FText::FromString(TEXT("50 / 100"));
	}

	float Skill = StatsSubsystem->GetDrivingSkill();
	return FText::FromString(FString::Printf(TEXT("%.0f / 100"), Skill));
}

FText SMZStatsWidget::GetEconomyText() const
{
	UGameInstance* GI = nullptr;
	if (StatsSubsystem.IsValid())
	{
		GI = StatsSubsystem->GetGameInstance();
	}

	if (!GI)
	{
		return FText::FromString(TEXT("N/A"));
	}

	UMZEconomySubsystem* EconomySubsystem = GI->GetSubsystem<UMZEconomySubsystem>();
	if (!EconomySubsystem)
	{
		return FText::FromString(TEXT("N/A"));
	}

	float TotalEarned, TotalSpent;
	EconomySubsystem->GetSessionEconomics(TotalEarned, TotalSpent);

	return FText::FromString(FString::Printf(TEXT("+$%.2f / -$%.2f (Balance: $%.2f)"),
		TotalEarned, TotalSpent, EconomySubsystem->GetCurrentMoney()));
}

TSharedRef<SWidget> SMZStatsWidget::MakeStatRow(const FString& Label, const FText& Value)
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(15.0f, 5.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(Label))
		.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
		.ColorAndOpacity(FSlateColor(FLinearColor::White))
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(5.0f)
	[
		SNew(STextBlock)
		.Text(Value)
		.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14))
		.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
	];
}

FReply SMZStatsWidget::OnCloseClicked()
{
	CloseStats();
	return FReply::Handled();
}
