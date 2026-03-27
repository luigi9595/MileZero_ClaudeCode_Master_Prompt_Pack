#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZWeatherTypes.h"
#include "MZWeatherManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMZOnWeatherChanged);

/**
 * Manages weather state and surface wetness for the MileZero world.
 * Drives grip modification on surfaces and broadcasts weather change events.
 * Placed directly in the level; automatically ticks to transition wetness.
 */
UCLASS()
class MILEZERO_API AMZWeatherManager : public AActor
{
	GENERATED_BODY()

public:
	AMZWeatherManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Configuration ---

	/** Current weather state (Dry, LightRain, HeavyRain) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Weather")
	EMZWeatherState CurrentWeather = EMZWeatherState::Dry;

	/** Target wetness value that we lerp toward (0-1) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Weather")
	float TargetWetness = 0.0f;

	/** Current wetness factor (0 = dry, 1 = fully wet) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MZ|Weather")
	float WetnessFactor = 0.0f;

	/** How fast wetness changes per second (lerp speed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Weather", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float WetnessChangeRate = 0.1f;

	// --- API ---

	/**
	 * Set the weather state and update target wetness accordingly.
	 * Transition is smooth via WetnessChangeRate.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Weather")
	void SetWeather(EMZWeatherState NewWeather);

	/**
	 * Get the current surface grip modifier based on wetness.
	 * Returns 1.0 when dry, scales down to ~0.78 when fully wet (simulating slippery conditions).
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Weather")
	float GetSurfaceGripModifier() const;

	/**
	 * Get raw wetness factor (0-1) for any custom calculations.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Weather")
	float GetWetnessFactor() const { return WetnessFactor; }

	/**
	 * Check if it's currently raining.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Weather")
	bool IsRaining() const { return CurrentWeather != EMZWeatherState::Dry; }

	// --- Events ---

	/** Broadcast when weather changes */
	UPROPERTY(BlueprintAssignable, Category = "MZ|Weather")
	FMZOnWeatherChanged OnWeatherChanged;

protected:
	/** Last recorded weather to detect changes */
	EMZWeatherState LastWeather = EMZWeatherState::Dry;

	/** Map weather state to target wetness value */
	float GetTargetWetnessForWeather(EMZWeatherState Weather) const;
};
