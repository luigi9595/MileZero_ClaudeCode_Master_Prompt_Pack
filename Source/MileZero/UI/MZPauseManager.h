#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZPauseManager.generated.h"

class SMZPauseMenuWidget;

/**
 * UMZPauseManager: Manages pause state and pause menu widget lifecycle.
 * Handles Escape key input, shows/hides pause menu, and sets game paused state.
 */
UCLASS()
class MILEZERO_API UMZPauseManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Toggle pause state (show/hide pause menu).
	 * Sets bShowMouseCursor and sets game paused state accordingly.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Pause")
	void TogglePause();

	/**
	 * Check if the game is currently paused.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Pause")
	bool IsPaused() const { return bIsPaused; }

	/**
	 * Resume the game (hide pause menu).
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Pause")
	void Resume();

	/**
	 * Quit to desktop.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Pause")
	void QuitToDesktop();

private:
	bool bIsPaused = false;

	TSharedPtr<SMZPauseMenuWidget> PauseMenuWidget;

	/**
	 * Create and show the pause menu widget.
	 */
	void ShowPauseMenu();

	/**
	 * Hide and destroy the pause menu widget.
	 */
	void HidePauseMenu();
};
