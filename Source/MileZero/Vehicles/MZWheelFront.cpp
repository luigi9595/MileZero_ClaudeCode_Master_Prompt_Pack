#include "MZWheelFront.h"

UMZWheelFront::UMZWheelFront()
{
	WheelRadius = 40.0f;
	CorneringStiffness = 750.0f;
	FrictionForceMultiplier = 4.0f;
	bAffectedByEngine = true;

	SuspensionMaxRaise = 15.0f;
	SuspensionMaxDrop = 15.0f;
	WheelLoadRatio = 1.0f;
	SpringRate = 120.0f;
	SpringPreload = 100.0f;
	SweepShape = ESweepShape::Shapecast;

	MaxBrakeTorque = 3000.0f;
	MaxHandBrakeTorque = 0.0f; // front wheels don't handbrake
}
