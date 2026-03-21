#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MZVehicleDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMZDrivetrainType : uint8
{
	RWD UMETA(DisplayName = "Rear-Wheel Drive"),
	FWD UMETA(DisplayName = "Front-Wheel Drive"),
	AWD UMETA(DisplayName = "All-Wheel Drive")
};

USTRUCT(BlueprintType)
struct FMZWheelSetup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float SpringRate = 25000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float DampingRateBump = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float DampingRateRebound = 4500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float SuspensionMaxDrop = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float SuspensionMaxRaise = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel")
	float WheelRadius = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel")
	float WheelWidth = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel")
	float LateralStiffness = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel")
	float LongitudinalStiffness = 1000.0f;
};

USTRUCT(BlueprintType)
struct FMZEngineSetup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	float MaxTorque = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	float MaxRPM = 7000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	float IdleRPM = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	float EngineRevDownRate = 2000.0f;
};

USTRUCT(BlueprintType)
struct FMZBrakeSetup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brakes")
	float MaxBrakeTorque = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brakes")
	float HandbrakeTorque = 5000.0f;

	// Front brake bias (0.0 = all rear, 1.0 = all front)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brakes", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FrontBrakeBias = 0.6f;
};

UCLASS(BlueprintType)
class MILEZERO_API UMZVehicleDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Identity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName VehicleID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	// Mass and dimensions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
	float MassKg = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
	FVector CenterOfMassOffset = FVector(0.0f, 0.0f, -15.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
	float DragCoefficient = 0.35f;

	// Drivetrain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drivetrain")
	EMZDrivetrainType DrivetrainType = EMZDrivetrainType::FWD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drivetrain")
	TArray<float> GearRatios;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drivetrain")
	float FinalDriveRatio = 3.5f;

	// Steering
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Steering")
	float MaxSteerAngle = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Steering")
	float SteeringSpeed = 5.0f;

	// Engine
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	FMZEngineSetup Engine;

	// Brakes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brakes")
	FMZBrakeSetup Brakes;

	// Wheels — front pair and rear pair share setup within each axle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheels")
	FMZWheelSetup FrontWheels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheels")
	FMZWheelSetup RearWheels;

	// Mesh reference (soft path to avoid hard asset dependency)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TSoftObjectPtr<USkeletalMesh> VehicleMesh;

	// Primary Asset ID support
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MZVehicle", VehicleID);
	}
};
