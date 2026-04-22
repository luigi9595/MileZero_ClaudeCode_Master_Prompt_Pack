#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSlider.h"

class UMZPauseManager;
class STextBlock;

/**
 * SMZPauseMenuWidget: Slate widget for the pause menu.
 * Displays centered menu with Resume, Stats, Settings, Save, Load, and Quit buttons.
 * Includes inline settings panel for audio, telemetry, and camera settings.
 */
class MILEZERO_API SMZPauseMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMZPauseMenuWidget)
		: _PauseManager(nullptr)
	{}
		SLATE_ARGUMENT(UMZPauseManager*, PauseManager)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	// Button callbacks
	FReply OnResumeClicked();
	FReply OnStatsClicked();
	FReply OnSettingsClicked();
	FReply OnSaveGameClicked();
	FReply OnLoadGameClicked();
	FReply OnQuitClicked();

	// Settings panel callbacks
	void OnMusicVolumeChanged(float Value);
	void OnSFXVolumeChanged(float Value);
	void OnTelemetryToggled(ECheckBoxState NewState);
	void OnMouseSensitivityChanged(float Value);
	void OnCameraFOVChanged(float Value);

	UMZPauseManager* CachedPauseManager = nullptr;

	// Settings widgets
	TSharedPtr<SSlider> MusicVolumeSlider;
	TSharedPtr<SSlider> SFXVolumeSlider;
	TSharedPtr<SSlider> MouseSensitivitySlider;
	TSharedPtr<SSlider> CameraFOVSlider;
	TSharedPtr<STextBlock> SettingsPanelContent;

	bool bShowSettings = false;
	bool bShowStats = false;
};
