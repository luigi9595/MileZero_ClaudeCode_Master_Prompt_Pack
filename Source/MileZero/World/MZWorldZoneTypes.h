#pragma once

#include "CoreMinimal.h"

/**
 * EMZZoneType: Enum of world zone types for MileZero.
 * Each zone has distinct handling for speed limits, surface types, and traffic behavior.
 */
UENUM(BlueprintType)
enum class EMZZoneType : uint8
{
	City UMETA(DisplayName = "City"),
	Industrial UMETA(DisplayName = "Industrial"),
	Highway UMETA(DisplayName = "Highway"),
	Rural UMETA(DisplayName = "Rural"),
	ProvingGround UMETA(DisplayName = "Proving Ground")
};
