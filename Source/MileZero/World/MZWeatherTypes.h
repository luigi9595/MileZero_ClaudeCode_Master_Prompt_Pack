#pragma once

#include "CoreMinimal.h"
#include "MZWeatherTypes.generated.h"

/**
 * Weather state enumeration for the MileZero world
 * Affects surface grip and visual/audio conditions
 */
UENUM(BlueprintType)
enum class EMZWeatherState : uint8
{
	Dry UMETA(DisplayName = "Dry"),
	LightRain UMETA(DisplayName = "Light Rain"),
	HeavyRain UMETA(DisplayName = "Heavy Rain")
};
