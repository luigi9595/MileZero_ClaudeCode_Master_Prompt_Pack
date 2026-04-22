#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SSlider;
class STextBlock;
class SButton;
class UMZPhotoMode;

/**
 * Slate widget for photo mode UI overlay.
 * Shows effect sliders, instructions, and controls.
 * Dark semi-transparent styling for clean screenshots.
 */
class MILEZERO_API SMZPhotoModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZPhotoModeWidget)
		: _PhotoModeSubsystem(nullptr)
	{
	}
		SLATE_ARGUMENT(UMZPhotoMode*, PhotoModeSubsystem)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

	/**
	 * Update all UI sliders to reflect current effect values
	 */
	void RefreshEffectValues();

	/**
	 * Toggle UI visibility
	 */
	void ToggleUIVisibility();

protected:
	TWeakObjectPtr<UMZPhotoMode> PhotoModeSubsystem;

	TSharedPtr<SSlider> ExposureSlider;
	TSharedPtr<SSlider> ContrastSlider;
	TSharedPtr<SSlider> SaturationSlider;
	TSharedPtr<SSlider> VignetteSlider;
	TSharedPtr<SSlider> FocusDistanceSlider;

	TSharedPtr<STextBlock> FOVText;
	TSharedPtr<STextBlock> ExposureValueText;
	TSharedPtr<STextBlock> ContrastValueText;
	TSharedPtr<STextBlock> SaturationValueText;
	TSharedPtr<STextBlock> VignetteValueText;
	TSharedPtr<STextBlock> FocusDistanceValueText;

	bool bUIVisible;

	// --- Slider Callbacks ---

	void OnExposureChanged(float NewValue);
	void OnContrastChanged(float NewValue);
	void OnSaturationChanged(float NewValue);
	void OnVignetteChanged(float NewValue);
	void OnFocusDistanceChanged(float NewValue);

	// --- Button Callbacks ---

	FReply OnResetEffectsClicked();
	FReply OnResetCameraClicked();
	FReply OnTakeScreenshotClicked();
};
