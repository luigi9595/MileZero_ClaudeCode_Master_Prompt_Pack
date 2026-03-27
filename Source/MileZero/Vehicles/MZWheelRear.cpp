#include "MZWheelRear.h"

UMZWheelRear::UMZWheelRear()
{
	WheelRadius = 40.0f;
	CorneringStiffness = 750.0f;
	FrictionForceMultiplier = 4.0f;
	bAffectedByEngine = false; // rear wheels on FWD (overridden by data asset)

	SuspensionMaxRaise = 15.0f;
	SuspensionMaxDrop = 15.0f;
	WheelLoadRatio = 1.0f;
	SpringRate = 120.0f;
	SpringPreload = 100.0f;
	SweepShape = ESweepShape::Shapecast;

	MaxBrakeTorque = 2000.0f;
	MaxHandBrakeTorque = 6000.0f;
}
