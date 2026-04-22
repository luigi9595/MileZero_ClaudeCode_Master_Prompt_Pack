#pragma once

#include "CoreMinimal.h"
#include "MZPedestrianTypes.generated.h"

/**
 * State enum for pedestrian AI behavior
 */
UENUM(BlueprintType)
enum class EMZPedestrianState : uint8
{
	Idle = 0,     // Standing in place or paused
	Walking = 1,  // Moving at normal pace
	Running = 2,  // Moving at fast pace
	Fleeing = 3   // Running away from danger (vehicle)
};
