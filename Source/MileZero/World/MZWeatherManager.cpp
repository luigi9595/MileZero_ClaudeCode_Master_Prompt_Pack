#include "MZWeatherManager.h"
#include "MileZero.h"

AMZWeatherManager::AMZWeatherManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.05f;
	bNetLoadOnClient = false;
}

void AMZWeatherManager::BeginPlay()
{
	Super::BeginPlay();

	// Initialize wetness based on current weather
	TargetWetness = GetTargetWetnessForWeather(CurrentWeather);
	WetnessFactor = TargetWetness;
	LastWeather = CurrentWeather;

	UE_LOG(LogMileZero, Log, TEXT("MZWeatherManager initialized. Weather: %d, Wetness: %.2f"),
		static_cast<int32>(CurrentWeather), WetnessFactor);
}

void AMZWeatherManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if weather changed
	if (CurrentWeather != LastWeather)
	{
		LastWeather = CurrentWeather;
		TargetWetness = GetTargetWetnessForWeather(CurrentWeather);
		OnWeatherChanged.Broadcast();
		UE_LOG(LogMileZero, Log, TEXT("Weather changed to %d. Target wetness: %.2f"),
			static_cast<int32>(CurrentWeather), TargetWetness);
	}

	// Lerp wetness toward target
	if (!FMath::IsNearlyEqual(WetnessFactor, TargetWetness, 0.001f))
	{
		WetnessFactor = FMath::Lerp(WetnessFactor, TargetWetness, WetnessChangeRate * DeltaTime);
	}
}

void AMZWeatherManager::SetWeather(EMZWeatherState NewWeather)
{
	CurrentWeather = NewWeather;
}

float AMZWeatherManager::GetSurfaceGripModifier() const
{
	// Dry: 1.0, Fully wet: 0.78 (approximately)
	// Linear interpolation: 1.0 - (0.22 * wetness)
	return 1.0f - (0.22f * WetnessFactor);
}

float AMZWeatherManager::GetTargetWetnessForWeather(EMZWeatherState Weather) const
{
	switch (Weather)
	{
	case EMZWeatherState::Dry:
		return 0.0f;
	case EMZWeatherState::LightRain:
		return 0.4f;
	case EMZWeatherState::HeavyRain:
		return 1.0f;
	default:
		return 0.0f;
	}
}
