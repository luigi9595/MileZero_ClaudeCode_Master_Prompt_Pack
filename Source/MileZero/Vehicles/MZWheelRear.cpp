#include "MZWheelRear.h"

UMZWheelRear::UMZWheelRear()
{
	AxleType = EAxleType::Rear;
	MaxSteerAngle = 0.0f;

	WheelRadius = 36.0f;
	WheelWidth = 28.0f;
	CorneringStiffness = 1000.0f;
	FrictionForceMultiplier = 3.5f;
	SideSlipModifier = 1.0f;
	SlipThreshold = 20.0f;
	SkidThreshold = 20.0f;
	bAffectedByEngine = true; // default RWD; data asset overrides

	// Slightly stiffer rear for neutral balance
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionDampingRatio = 0.85f;
	WheelLoadRatio = 0.5f;
	SpringRate = 270.0f;
	SpringPreload = 50.0f;
	SuspensionSmoothing = 6;
	RollbarScaling = 0.6f;
	SweepShape = ESweepShape::Shapecast;
	SuspensionForceOffset = FVector(0.0f, 0.0f, 0.0f);

	MaxBrakeTorque = 2500.0f;
	MaxHandBrakeTorque = 5000.0f;
	bABSEnabled = true;
}
