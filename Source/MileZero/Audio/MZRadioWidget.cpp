#include "MZRadioWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateColor.h"
#include "../MileZero.h"

void SMZRadioWidget::Construct(const FArguments& InArgs)
{
	DisplayDuration = 3.0f;
	DisplayStartTime = -999.0f;
	CurrentOpacity = 0.0f;
	bIsVisible = false;

	ChildSlot
	[
		SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0.0f, 20.0f, 0.0f, 0.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SAssignNew(StationNameText, STextBlock)
				.Text(FText::FromString(TEXT("---")))
				.Font(FAppStyle::GetFontStyle("HeadingSmall"))
				.ColorAndOpacity(FLinearColor::White)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
			[
				SAssignNew(GenreText, STextBlock)
				.Text(FText::FromString(TEXT("---")))
				.Font(FAppStyle::GetFontStyle("SmallText"))
				.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
			[
				SAssignNew(TrackText, STextBlock)
				.Text(FText::FromString(TEXT("No tracks loaded")))
				.Font(FAppStyle::GetFontStyle("SmallText"))
				.ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f))
			]
		]
	];
}

void SMZRadioWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bIsVisible)
	{
		float ElapsedTime = InCurrentTime - DisplayStartTime;

		// Fade out after display duration
		if (ElapsedTime > DisplayDuration)
		{
			bIsVisible = false;
			CurrentOpacity = 0.0f;
		}
		else
		{
			// Linear interpolation for opacity during display window
			CurrentOpacity = 1.0f;

			// Optional: fade out in last 0.5 seconds
			if (ElapsedTime > (DisplayDuration - 0.5f))
			{
				float FadeOutTime = ElapsedTime - (DisplayDuration - 0.5f);
				CurrentOpacity = FMath::Lerp(1.0f, 0.0f, FadeOutTime / 0.5f);
			}
		}
	}
}

void SMZRadioWidget::ShowStation(const FMZRadioStation& Station)
{
	if (StationNameText.IsValid())
	{
		StationNameText->SetText(Station.StationName);
	}

	if (GenreText.IsValid())
	{
		GenreText->SetText(Station.Genre);
	}

	if (TrackText.IsValid())
	{
		if (Station.TrackIDs.IsValidIndex(Station.CurrentTrackIndex))
		{
			FString TrackName = Station.TrackIDs[Station.CurrentTrackIndex].ToString();
			TrackText->SetText(FText::FromString(FString::Printf(TEXT("Track: %s"), *TrackName)));
		}
		else
		{
			TrackText->SetText(FText::FromString(TEXT("No tracks loaded")));
		}
	}

	DisplayStartTime = FSlateApplication::Get().GetCurrentTime();
	bIsVisible = true;
	CurrentOpacity = 1.0f;

	UE_LOG(LogMileZero, Log, TEXT("Radio widget showing: %s (%s)"), *Station.StationName.ToString(), *Station.Genre.ToString());
}

void SMZRadioWidget::Hide()
{
	bIsVisible = false;
	CurrentOpacity = 0.0f;
}
