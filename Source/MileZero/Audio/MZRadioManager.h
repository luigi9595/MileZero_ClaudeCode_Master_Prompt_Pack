#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZRadioTypes.h"
#include "MZRadioManager.generated.h"

class UAudioComponent;

/**
 * Game instance subsystem managing radio stations and music playback.
 * Provides station cycling, track management, and delegates for UI updates.
 * Audio playback is stubbed until audio assets are available.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStationChangedDelegate, const FMZRadioStation&, NewStation, int32, StationIndex);

UCLASS()
class MILEZERO_API UMZRadioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- Configuration (set up in Initialize) ---

	/** List of all available radio stations */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|Radio")
	TArray<FMZRadioStation> Stations;

	/** Whether radio is currently enabled (user can toggle) */
	UPROPERTY(BlueprintReadWrite, Category = "MZ|Radio")
	bool bRadioEnabled = true;

	// --- State ---

	/** Currently tuned station index */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|Radio")
	int32 CurrentStationIndex = 0;

	// --- Delegates ---

	/** Called when station changes (new station, index) */
	UPROPERTY(BlueprintAssignable, Category = "MZ|Radio")
	FOnStationChangedDelegate OnStationChanged;

	// --- API ---

	/**
	 * Cycle to next station (wraps around at end)
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Radio")
	void NextStation();

	/**
	 * Cycle to previous station (wraps around at beginning)
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Radio")
	void PrevStation();

	/**
	 * Get the currently tuned station
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Radio")
	FMZRadioStation GetCurrentStation() const;

	/**
	 * Get name of current station (convenience accessor)
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Radio")
	FText GetCurrentStationName() const;

	/**
	 * Get genre of current station
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Radio")
	FText GetCurrentStationGenre() const;

	/**
	 * Tune to specific station by ID
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Radio")
	void TuneToStation(FName StationID);

	/**
	 * Get station by ID
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Radio")
	FMZRadioStation GetStationByID(FName StationID) const;

	/**
	 * Enable/disable radio playback
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Radio")
	void SetRadioEnabled(bool bEnabled);

protected:
	// --- Internal State ---

	TObjectPtr<UAudioComponent> RadioAudioComponent;

	// --- Initialization ---

	void SetupDefaultStations();
};
