#include "MZPauseMenuWidget.h"
#include "MZPauseManager.h"
#include "MileZero.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSlider.h"
#include "Kismet/GameplayStatics.h"

void SMZPauseMenuWidget::Construct(const FArguments& InArgs)
{
	CachedPauseManager = InArgs._PauseManager;

	ChildSlot
	[
		SNew(SOverlay)
		// Dark overlay background
		+ SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f))
		]
		// Pause menu panel
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.95f))
			.Padding(FMargin(30.0f, 40.0f))
			[
				SNew(SVerticalBox)
				// Title
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(0.0f, 0.0f, 0.0f, 20.0f)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 48))
					.Text(FText::FromString(TEXT("MILEZERO")))
					.ColorAndOpacity(FLinearColor::Yellow)
					.ShadowOffset(FVector2D(2, 2))
					.ShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.8f))
				]
				// Resume button
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f)
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
					.OnClicked(this, &SMZPauseMenuWidget::OnResumeClicked)
					[
						SNew(SBox)
						.Padding(FMargin(20.0f, 10.0f))
						[
							SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.Text(FText::FromString(TEXT("Resume")))
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
				// Stats button
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f)
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
					.OnClicked(this, &SMZPauseMenuWidget::OnStatsClicked)
					[
						SNew(SBox)
						.Padding(FMargin(20.0f, 10.0f))
						[
							SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.Text(FText::FromString(TEXT("Stats")))
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
				// Settings button
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f)
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
					.OnClicked(this, &SMZPauseMenuWidget::OnSettingsClicked)
					[
						SNew(SBox)
						.Padding(FMargin(20.0f, 10.0f))
						[
							SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.Text(FText::FromString(TEXT("Settings")))
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
				// Save Game button
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f)
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
					.OnClicked(this, &SMZPauseMenuWidget::OnSaveGameClicked)
					[
						SNew(SBox)
						.Padding(FMargin(20.0f, 10.0f))
						[
							SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.Text(FText::FromString(TEXT("Save Game")))
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
				// Load Game button
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f)
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
					.OnClicked(this, &SMZPauseMenuWidget::OnLoadGameClicked)
					[
						SNew(SBox)
						.Padding(FMargin(20.0f, 10.0f))
						[
							SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.Text(FText::FromString(TEXT("Load Game")))
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
				// Quit button
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f)
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor(0.3f, 0.1f, 0.1f, 1.0f))
					.OnClicked(this, &SMZPauseMenuWidget::OnQuitClicked)
					[
						SNew(SBox)
						.Padding(FMargin(20.0f, 10.0f))
						[
							SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.Text(FText::FromString(TEXT("Quit to Desktop")))
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
			]
		]
	];
}

FReply SMZPauseMenuWidget::OnResumeClicked()
{
	if (CachedPauseManager)
	{
		CachedPauseManager->Resume();
	}
	UE_LOG(LogMileZero, Log, TEXT("Resume button clicked"));
	return FReply::Handled();
}

FReply SMZPauseMenuWidget::OnStatsClicked()
{
	bShowStats = !bShowStats;
	UE_LOG(LogMileZero, Log, TEXT("Stats button clicked (stub)"));
	return FReply::Handled();
}

FReply SMZPauseMenuWidget::OnSettingsClicked()
{
	bShowSettings = !bShowSettings;
	UE_LOG(LogMileZero, Log, TEXT("Settings button clicked (stub)"));
	return FReply::Handled();
}

FReply SMZPauseMenuWidget::OnSaveGameClicked()
{
	UE_LOG(LogMileZero, Log, TEXT("Save Game button clicked"));

	if (UWorld* World = GEngine->GetWorldContexts().Num() > 0
		? GEngine->GetWorldContexts()[0].World() : nullptr)
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
		{
			// Save game would be called here on save manager
			UE_LOG(LogMileZero, Log, TEXT("Save game triggered (integration pending)"));
		}
	}

	return FReply::Handled();
}

FReply SMZPauseMenuWidget::OnLoadGameClicked()
{
	UE_LOG(LogMileZero, Log, TEXT("Load Game button clicked"));

	if (UWorld* World = GEngine->GetWorldContexts().Num() > 0
		? GEngine->GetWorldContexts()[0].World() : nullptr)
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
		{
			// Load game would be called here on save manager
			UE_LOG(LogMileZero, Log, TEXT("Load game triggered (integration pending)"));
		}
	}

	return FReply::Handled();
}

FReply SMZPauseMenuWidget::OnQuitClicked()
{
	UE_LOG(LogMileZero, Log, TEXT("Quit button clicked"));
	if (CachedPauseManager)
	{
		CachedPauseManager->QuitToDesktop();
	}
	return FReply::Handled();
}

void SMZPauseMenuWidget::OnMusicVolumeChanged(float Value)
{
	UE_LOG(LogMileZero, Log, TEXT("Music volume changed to %.0f%%"), Value * 100.0f);
}

void SMZPauseMenuWidget::OnSFXVolumeChanged(float Value)
{
	UE_LOG(LogMileZero, Log, TEXT("SFX volume changed to %.0f%%"), Value * 100.0f);
}

void SMZPauseMenuWidget::OnTelemetryToggled(ECheckBoxState NewState)
{
	UE_LOG(LogMileZero, Log, TEXT("Telemetry toggle: %s"), NewState == ECheckBoxState::Checked ? TEXT("ON") : TEXT("OFF"));
}

void SMZPauseMenuWidget::OnMouseSensitivityChanged(float Value)
{
	UE_LOG(LogMileZero, Log, TEXT("Mouse sensitivity changed to %.2f"), Value);
}

void SMZPauseMenuWidget::OnCameraFOVChanged(float Value)
{
	UE_LOG(LogMileZero, Log, TEXT("Camera FOV changed to %.0f"), Value);
}
