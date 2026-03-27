#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZSaveGame.h"
#include "MZSaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveStateChanged, bool, bSuccess, FString, Message);

/**
 * Manages game save and load operations.
 * Handles persistent data serialization and retrieval.
 */
UCLASS()
class MILEZERO_API UMZSaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Save current game state to slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool SaveGame(const FString& SlotName = "");

	/**
	 * Load game from slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool LoadGame(const FString& SlotName = "");

	/**
	 * Auto-save to default slot (called periodically or on important events)
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool AutoSave();

	/**
	 * Check if save data exists
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool HasSaveData(const FString& SlotName = "") const;

	/**
	 * Delete save slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool DeleteSave(const FString& SlotName = "");

	/**
	 * Get the current save game object
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	UMZSaveGame* GetCurrentSaveGame() const { return CurrentSaveGame; }

	/**
	 * Update playtime and trigger autosave
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void UpdatePlaytime(float DeltaTime);

	// Delegate for save state changes
	UPROPERTY(BlueprintAssignable, Category = "Save")
	FOnSaveStateChanged OnSaveStateChanged;

protected:
	// Current loaded save game
	UPROPERTY(BlueprintReadOnly, Category = "Save")
	TObjectPtr<UMZSaveGame> CurrentSaveGame;

	// Accumulated playtime since last save (to avoid constant saves)
	float PlaytimeAccumulator = 0.0f;

	// Autosave interval in seconds
	float AutosaveIntervalSeconds = 60.0f;

	/**
	 * Capture current game state into save game object
	 */
	void CaptureGameState(UMZSaveGame* SaveGame);

	/**
	 * Apply saved game state to world
	 */
	void ApplyGameState(const UMZSaveGame* SaveGame);

	/**
	 * Get the active slot name
	 */
	FString GetActiveSlotName(const FString& SlotName) const;
};
