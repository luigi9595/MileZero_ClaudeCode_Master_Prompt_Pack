#include "MZWheelFront.h"

UMZWheelFront::UMZWheelFront()
{
	AxleType = EAxleType::Front;
	MaxSteerAngle = 35.0f;

	WheelRadius = 36.0f;
	WheelWidth = 24.0f;
	CorneringStiffness = 1200.0f;
	FrictionForceMultiplier = 3.5f;
	SideSlipModifier = 1.0f;
	SlipThreshold = 20.0f;
	SkidThreshold = 20.0f;
	bAffectedByEngine = false; // default FWD-safe; data asset overrides

	// Realistic sports car suspension
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionDampingRatio = 0.8f;
	WheelLoadRatio = 0.5f;
	SpringRate = 250.0f;
	SpringPreload = 50.0f;
	SuspensionSmoothing = 6;
	RollbarScaling = 0.5f;
	SweepShape = ESweepShape::Shapecast;
	SuspensionForceOffset = FVector(0.0f, 0.0f, 0.0f);

	MaxBrakeTorque = 4000.0f;
	MaxHandBrakeTorque = 0.0f;
	bABSEnabled = true;
}
