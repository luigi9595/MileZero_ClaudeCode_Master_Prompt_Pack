#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZTimeOfDayManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMZOnTimeEvent);

class UDirectionalLightComponent;

/**
 * Manages time of day, sun rotation, and lighting for the MileZero world.
 * Advances game time, rotates the sun actor, and blends between day/sunset/night colors.
 * Placed directly in the level with a reference to the sun light.
 */
UCLASS()
class MILEZERO_API AMZTimeOfDayManager : public AActor
{
	GENERATED_BODY()

public:
	AMZTimeOfDayManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Configuration ---

	/** Current time of day (0-24, where 12 = noon, 0/24 = midnight) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay")
	float TimeOfDay = 12.0f;

	/** Game time scale: how many game seconds pass per real second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay", meta = (ClampMin = "0.1", ClampMax = "3600.0"))
	float TimeScale = 60.0f;

	/** Pause time advancement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay")
	bool bPaused = false;

	/** Reference to the directional light in the scene (find or set in editor) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay")
	TObjectPtr<UDirectionalLightComponent> SunLight;

	/** Hour when sun rises (e.g., 6.0 = 6 AM) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float SunriseHour = 6.0f;

	/** Hour when sun sets (e.g., 20.0 = 8 PM) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float SunsetHour = 20.0f;

	/** Color during day (brightest) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay")
	FLinearColor DayColor = FLinearColor::White;

	/** Color at sunset (orange/red tint) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay")
	FLinearColor SunsetColor = FLinearColor(1.0f, 0.6f, 0.2f, 1.0f);

	/** Color at night (dark blue) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|TimeOfDay")
	FLinearColor NightColor = FLinearColor(0.05f, 0.05f, 0.2f, 1.0f);

	// --- API ---

	/**
	 * Get the sun's rotation based on current time.
	 * Maps 0-24 hours to a full 360° rotation around the sky.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|TimeOfDay")
	FRotator GetSunRotation() const;

	/**
	 * Get the sun's intensity based on time of day.
	 * Peak at noon, zero at midnight, transitional at sunrise/sunset.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|TimeOfDay")
	float GetSunIntensity() const;

	/**
	 * Get the current ambient/sky color for the time of day.
	 * Blends between day, sunset, and night colors.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|TimeOfDay")
	FLinearColor GetTimeOfDayColor() const;

	/**
	 * Check if it's currently daytime.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|TimeOfDay")
	bool IsDaytime() const;

	/**
	 * Check if it's currently nighttime.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|TimeOfDay")
	bool IsNighttime() const;

	/**
	 * Get normalized time of day (0-1, where 0.5 = noon).
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|TimeOfDay")
	float GetNormalizedTimeOfDay() const { return TimeOfDay / 24.0f; }

	/**
	 * Get the current time as a formatted string (HH:MM).
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|TimeOfDay")
	FString GetTimeString() const;

	// --- Events ---

	/** Called when sun rises */
	UPROPERTY(BlueprintAssignable, Category = "MZ|TimeOfDay")
	FMZOnTimeEvent OnSunrise;

	/** Called when sun sets */
	UPROPERTY(BlueprintAssignable, Category = "MZ|TimeOfDay")
	FMZOnTimeEvent OnSunset;

	/** Called at midnight */
	UPROPERTY(BlueprintAssignable, Category = "MZ|TimeOfDay")
	FMZOnTimeEvent OnMidnight;

	/** Called at noon */
	UPROPERTY(BlueprintAssignable, Category = "MZ|TimeOfDay")
	FMZOnTimeEvent OnNoon;

protected:
	/** Track if sunrise event was already broadcast today */
	bool bSunriseEventFired = false;

	/** Track if sunset event was already broadcast today */
	bool bSunsetEventFired = false;

	/** Track if midnight event was already broadcast today */
	bool bMidnightEventFired = false;

	/** Track if noon event was already broadcast today */
	bool bNoonEventFired = false;
};
