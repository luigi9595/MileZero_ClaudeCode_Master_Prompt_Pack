#include "MZPhotoModeWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
// SHorizontalBox included via SBoxPanel.h
#include "Widgets/Layout/SSpacer.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateColor.h"
#include "MZPhotoMode.h"
#include "../MileZero.h"

void SMZPhotoModeWidget::Construct(const FArguments& InArgs)
{
	PhotoModeSubsystem = InArgs._PhotoModeSubsystem;
	bUIVisible = true;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			// Left panel with effects
			SNew(SBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(FMargin(20.0f, 20.0f, 0.0f, 0.0f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 15.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("PHOTO MODE")))
					.Font(FAppStyle::GetFontStyle("HeadingMedium"))
					.ColorAndOpacity(FLinearColor::Yellow)
				]

				// Exposure Slider
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 200.0f, 5.0f))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.4f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Exposure")))
						.Font(FAppStyle::GetFontStyle("SmallText"))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.6f)
					[
						SAssignNew(ExposureSlider, SSlider)
						.Value(0.5f)
						.MinValue(0.0f)
						.MaxValue(1.0f)
						.OnValueChanged(this, &SMZPhotoModeWidget::OnExposureChanged)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(FMargin(0.0f, -20.0f, 200.0f, 10.0f))
				[
					SAssignNew(ExposureValueText, STextBlock)
					.Text(FText::FromString(TEXT("0.0")))
					.Font(FAppStyle::GetFontStyle("SmallText"))
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				]

				// Contrast Slider
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 200.0f, 5.0f))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.4f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Contrast")))
						.Font(FAppStyle::GetFontStyle("SmallText"))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.6f)
					[
						SAssignNew(ContrastSlider, SSlider)
						.Value(0.5f)
						.MinValue(0.0f)
						.MaxValue(1.0f)
						.OnValueChanged(this, &SMZPhotoModeWidget::OnContrastChanged)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(FMargin(0.0f, -20.0f, 200.0f, 10.0f))
				[
					SAssignNew(ContrastValueText, STextBlock)
					.Text(FText::FromString(TEXT("1.0")))
					.Font(FAppStyle::GetFontStyle("SmallText"))
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				]

				// Saturation Slider
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 200.0f, 5.0f))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.4f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Saturation")))
						.Font(FAppStyle::GetFontStyle("SmallText"))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.6f)
					[
						SAssignNew(SaturationSlider, SSlider)
						.Value(0.5f)
						.MinValue(0.0f)
						.MaxValue(1.0f)
						.OnValueChanged(this, &SMZPhotoModeWidget::OnSaturationChanged)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(FMargin(0.0f, -20.0f, 200.0f, 10.0f))
				[
					SAssignNew(SaturationValueText, STextBlock)
					.Text(FText::FromString(TEXT("1.0")))
					.Font(FAppStyle::GetFontStyle("SmallText"))
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				]

				// Vignette Slider
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 200.0f, 5.0f))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.4f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Vignette")))
						.Font(FAppStyle::GetFontStyle("SmallText"))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.6f)
					[
						SAssignNew(VignetteSlider, SSlider)
						.Value(0.0f)
						.MinValue(0.0f)
						.MaxValue(1.0f)
						.OnValueChanged(this, &SMZPhotoModeWidget::OnVignetteChanged)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(FMargin(0.0f, -20.0f, 200.0f, 10.0f))
				[
					SAssignNew(VignetteValueText, STextBlock)
					.Text(FText::FromString(TEXT("0.0")))
					.Font(FAppStyle::GetFontStyle("SmallText"))
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				]

				// Focus Distance Slider
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 200.0f, 5.0f))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.4f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Focus Dist")))
						.Font(FAppStyle::GetFontStyle("SmallText"))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.6f)
					[
						SAssignNew(FocusDistanceSlider, SSlider)
						.Value(0.5f)
						.MinValue(0.0f)
						.MaxValue(1.0f)
						.OnValueChanged(this, &SMZPhotoModeWidget::OnFocusDistanceChanged)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(FMargin(0.0f, -20.0f, 200.0f, 20.0f))
				[
					SAssignNew(FocusDistanceValueText, STextBlock)
					.Text(FText::FromString(TEXT("1000.0")))
					.Font(FAppStyle::GetFontStyle("SmallText"))
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				]

				// Buttons
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 10.0f, 0.0f, 5.0f))
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Reset Effects")))
					.OnClicked(this, &SMZPhotoModeWidget::OnResetEffectsClicked)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 0.0f, 5.0f))
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Reset Camera")))
					.OnClicked(this, &SMZPhotoModeWidget::OnResetCameraClicked)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 10.0f, 0.0f, 5.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("[Space] Take Photo")))
					.Font(FAppStyle::GetFontStyle("SmallBold"))
					.ColorAndOpacity(FLinearColor::Yellow)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 0.0f, 5.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("[Esc] Exit")))
					.Font(FAppStyle::GetFontStyle("SmallBold"))
					.ColorAndOpacity(FLinearColor::Yellow)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 10.0f, 0.0f, 5.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("[H] Hide UI")))
					.Font(FAppStyle::GetFontStyle("SmallText"))
					.ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 0.0f, 5.0f))
				[
					SAssignNew(FOVText, STextBlock)
					.Text(FText::FromString(TEXT("FOV: 90")))
					.Font(FAppStyle::GetFontStyle("SmallText"))
					.ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f))
				]
			]
		]
	];
}

void SMZPhotoModeWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	RefreshEffectValues();
}

void SMZPhotoModeWidget::RefreshEffectValues()
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (!PhotoMode)
	{
		return;
	}

	// Update text displays
	if (ExposureValueText.IsValid())
	{
		ExposureValueText->SetText(FText::AsNumber(PhotoMode->Exposure, &FNumberFormattingOptions().SetMinimumFractionalDigits(1)));
	}

	if (ContrastValueText.IsValid())
	{
		ContrastValueText->SetText(FText::AsNumber(PhotoMode->Contrast, &FNumberFormattingOptions().SetMinimumFractionalDigits(1)));
	}

	if (SaturationValueText.IsValid())
	{
		SaturationValueText->SetText(FText::AsNumber(PhotoMode->Saturation, &FNumberFormattingOptions().SetMinimumFractionalDigits(1)));
	}

	if (VignetteValueText.IsValid())
	{
		VignetteValueText->SetText(FText::AsNumber(PhotoMode->Vignette, &FNumberFormattingOptions().SetMinimumFractionalDigits(2)));
	}

	if (FocusDistanceValueText.IsValid())
	{
		FocusDistanceValueText->SetText(FText::AsNumber(PhotoMode->FocusDistance, &FNumberFormattingOptions().SetMinimumFractionalDigits(0)));
	}

	if (FOVText.IsValid())
	{
		FOVText->SetText(FText::FromString(FString::Printf(TEXT("FOV: %.0f"), PhotoMode->CurrentFOV)));
	}
}

void SMZPhotoModeWidget::OnExposureChanged(float NewValue)
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->Exposure = FMath::Lerp(-2.0f, 2.0f, NewValue);
	}
}

void SMZPhotoModeWidget::OnContrastChanged(float NewValue)
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->Contrast = FMath::Lerp(0.5f, 2.0f, NewValue);
	}
}

void SMZPhotoModeWidget::OnSaturationChanged(float NewValue)
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->Saturation = FMath::Lerp(0.0f, 2.0f, NewValue);
	}
}

void SMZPhotoModeWidget::OnVignetteChanged(float NewValue)
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->Vignette = NewValue;
	}
}

void SMZPhotoModeWidget::OnFocusDistanceChanged(float NewValue)
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->FocusDistance = FMath::Lerp(10.0f, 10000.0f, NewValue);
	}
}

FReply SMZPhotoModeWidget::OnResetEffectsClicked()
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->ResetEffects();
		RefreshEffectValues();
	}
	return FReply::Handled();
}

FReply SMZPhotoModeWidget::OnResetCameraClicked()
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->ResetCamera();
	}
	return FReply::Handled();
}

FReply SMZPhotoModeWidget::OnTakeScreenshotClicked()
{
	UMZPhotoMode* PhotoMode = PhotoModeSubsystem.Get();
	if (PhotoMode)
	{
		PhotoMode->TakeScreenshot();
	}
	return FReply::Handled();
}

void SMZPhotoModeWidget::ToggleUIVisibility()
{
	bUIVisible = !bUIVisible;
}
