#include "MZTimeOfDayManager.h"
#include "MileZero.h"
#include "Components/LightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "EngineUtils.h"

AMZTimeOfDayManager::AMZTimeOfDayManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.05f;
	bNetLoadOnClient = false;
}

void AMZTimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();

	// If sun light not assigned, try to find it in the level
	if (!SunLight)
	{
		// Find the first directional light in the level
		for (TActorIterator<ALight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->IsA<ADirectionalLight>())
			{
				SunLight = Cast<UDirectionalLightComponent>(ActorItr->GetLightComponent());
				if (SunLight)
				{
					UE_LOG(LogMileZero, Warning, TEXT("MZTimeOfDayManager: Sun light was not assigned, auto-found directional light"));
					break;
				}
			}
		}
	}

	// Clamp time of day to valid range
	TimeOfDay = FMath::Fmod(TimeOfDay, 24.0f);
	if (TimeOfDay < 0.0f)
	{
		TimeOfDay += 24.0f;
	}

	UE_LOG(LogMileZero, Log, TEXT("MZTimeOfDayManager initialized. Time: %.1f, TimeScale: %.1f"), TimeOfDay, TimeScale);
}

void AMZTimeOfDayManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Advance time if not paused
	if (!bPaused)
	{
		TimeOfDay += (DeltaTime * TimeScale) / 3600.0f; // Convert seconds to hours

		// Wrap around 24-hour cycle
		if (TimeOfDay >= 24.0f)
		{
			TimeOfDay -= 24.0f;
			bMidnightEventFired = false;
			bNoonEventFired = false;
			bSunriseEventFired = false;
			bSunsetEventFired = false;
		}
	}

	// Update sun light if available
	if (SunLight)
	{
		FRotator SunRotation = GetSunRotation();
		SunLight->SetRelativeRotation(SunRotation);

		float Intensity = GetSunIntensity();
		SunLight->SetIntensity(Intensity);

		FLinearColor TimeColor = GetTimeOfDayColor();
		SunLight->SetLightColor(TimeColor);
	}

	// Fire events at specific times
	float HourFraction = FMath::Fmod(TimeOfDay, 1.0f);

	if (TimeOfDay >= SunriseHour && TimeOfDay < (SunriseHour + 0.1f) && !bSunriseEventFired)
	{
		bSunriseEventFired = true;
		OnSunrise.Broadcast();
		UE_LOG(LogMileZero, Log, TEXT("Sunrise event fired"));
	}

	if (TimeOfDay >= SunsetHour && TimeOfDay < (SunsetHour + 0.1f) && !bSunsetEventFired)
	{
		bSunsetEventFired = true;
		OnSunset.Broadcast();
		UE_LOG(LogMileZero, Log, TEXT("Sunset event fired"));
	}

	if (TimeOfDay >= 0.0f && TimeOfDay < 0.1f && !bMidnightEventFired)
	{
		bMidnightEventFired = true;
		OnMidnight.Broadcast();
		UE_LOG(LogMileZero, Log, TEXT("Midnight event fired"));
	}

	if (TimeOfDay >= 12.0f && TimeOfDay < 12.1f && !bNoonEventFired)
	{
		bNoonEventFired = true;
		OnNoon.Broadcast();
		UE_LOG(LogMileZero, Log, TEXT("Noon event fired"));
	}
}

FRotator AMZTimeOfDayManager::GetSunRotation() const
{
	// Map time of day to pitch rotation
	// 0:00 (midnight) = -90° (below horizon)
	// 6:00 (sunrise) = -30° (low)
	// 12:00 (noon) = 90° (directly overhead)
	// 18:00 (sunset) = -30° (low)
	// 24:00 (midnight) = -90° (below horizon)

	float NormalizedTime = TimeOfDay / 24.0f; // 0-1
	float Pitch = -90.0f + (FMath::Sin(NormalizedTime * 2.0f * PI) * 180.0f);

	// Yaw stays consistent (sun moves east to west)
	float Yaw = NormalizedTime * 360.0f;

	return FRotator(Pitch, Yaw, 0.0f);
}

float AMZTimeOfDayManager::GetSunIntensity() const
{
	// Intensity follows a smooth curve
	// Zero at midnight, peaks at noon, back to zero
	float NormalizedTime = TimeOfDay / 24.0f;
	float Intensity = FMath::Clamp(FMath::Sin(NormalizedTime * PI), 0.0f, 1.0f);

	// Reduce intensity at sunrise/sunset
	if (TimeOfDay >= (SunriseHour - 1.0f) && TimeOfDay < SunriseHour)
	{
		Intensity *= 0.5f; // Dim approaching sunrise
	}
	else if (TimeOfDay >= SunriseHour && TimeOfDay < (SunriseHour + 1.0f))
	{
		Intensity *= FMath::Lerp(0.5f, 1.0f, (TimeOfDay - SunriseHour)); // Brighten after sunrise
	}
	else if (TimeOfDay >= (SunsetHour - 1.0f) && TimeOfDay < SunsetHour)
	{
		Intensity *= FMath::Lerp(1.0f, 0.5f, (TimeOfDay - (SunsetHour - 1.0f))); // Dim before sunset
	}
	else if (TimeOfDay >= SunsetHour && TimeOfDay < (SunsetHour + 1.0f))
	{
		Intensity *= 0.2f; // Very dim after sunset
	}

	return FMath::Max(0.0f, Intensity);
}

FLinearColor AMZTimeOfDayManager::GetTimeOfDayColor() const
{
	float Hour = TimeOfDay;

	// Before sunrise: night
	if (Hour < SunriseHour)
	{
		return NightColor;
	}
	// Sunrise transition (one hour window)
	else if (Hour >= SunriseHour && Hour < (SunriseHour + 1.0f))
	{
		float T = (Hour - SunriseHour) / 1.0f;
		return FLinearColor::LerpUsingHSV(NightColor, DayColor, T);
	}
	// Day
	else if (Hour >= (SunriseHour + 1.0f) && Hour < (SunsetHour - 1.0f))
	{
		return DayColor;
	}
	// Sunset transition (one hour window)
	else if (Hour >= (SunsetHour - 1.0f) && Hour < SunsetHour)
	{
		float T = (Hour - (SunsetHour - 1.0f)) / 1.0f;
		return FLinearColor::LerpUsingHSV(DayColor, SunsetColor, T);
	}
	// Night transition (one hour after sunset)
	else if (Hour >= SunsetHour && Hour < (SunsetHour + 1.0f))
	{
		float T = (Hour - SunsetHour) / 1.0f;
		return FLinearColor::LerpUsingHSV(SunsetColor, NightColor, T);
	}
	// Night (rest of day until midnight)
	else
	{
		return NightColor;
	}
}

bool AMZTimeOfDayManager::IsDaytime() const
{
	return TimeOfDay >= SunriseHour && TimeOfDay < SunsetHour;
}

bool AMZTimeOfDayManager::IsNighttime() const
{
	return !IsDaytime();
}

FString AMZTimeOfDayManager::GetTimeString() const
{
	int32 Hours = static_cast<int32>(TimeOfDay);
	int32 Minutes = static_cast<int32>((TimeOfDay - Hours) * 60.0f);
	return FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
}
