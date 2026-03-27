#include "MZVehicleRegistry.h"
#include "MZVehicleDataAsset.h"
#include "MileZero.h"

void UMZVehicleRegistry::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CreateDefaultVehicles();

	UE_LOG(LogMileZero, Log, TEXT("MZVehicleRegistry initialized with %d vehicles"), VehicleMap.Num());
}

UMZVehicleDataAsset* UMZVehicleRegistry::GetVehicleData(FName VehicleID) const
{
	if (const TObjectPtr<UMZVehicleDataAsset>* Found = VehicleMap.Find(VehicleID))
	{
		return Found->Get();
	}
	UE_LOG(LogMileZero, Warning, TEXT("Vehicle not found: %s"), *VehicleID.ToString());
	return nullptr;
}

TArray<FName> UMZVehicleRegistry::GetAllVehicleIDs() const
{
	TArray<FName> IDs;
	VehicleMap.GenerateKeyArray(IDs);
	IDs.Sort([](const FName& A, const FName& B) { return A.FastLess(B); });
	return IDs;
}

UMZVehicleDataAsset* UMZVehicleRegistry::GetVehicleDataByDisplayName(const FText& DisplayName) const
{
	for (const auto& Pair : VehicleMap)
	{
		if (Pair.Value && Pair.Value->DisplayName.EqualToCaseIgnored(DisplayName))
		{
			return Pair.Value;
		}
	}
	return nullptr;
}

void UMZVehicleRegistry::CreateDefaultVehicles()
{
	// Create all three vehicle profiles programmatically
	if (UMZVehicleDataAsset* Coupe = CreateCoupeData())
	{
		VehicleMap.Add(FName("Coupe"), Coupe);
	}

	if (UMZVehicleDataAsset* Hatch = CreateHatchData())
	{
		VehicleMap.Add(FName("Hatch"), Hatch);
	}

	if (UMZVehicleDataAsset* Pickup = CreatePickupData())
	{
		VehicleMap.Add(FName("Pickup"), Pickup);
	}
}

UMZVehicleDataAsset* UMZVehicleRegistry::CreateCoupeData()
{
	UMZVehicleDataAsset* Data = NewObject<UMZVehicleDataAsset>(this, FName("DA_Coupe"));
	if (!Data)
	{
		return nullptr;
	}

	// Identity
	Data->VehicleID = FName("Coupe");
	Data->DisplayName = FText::FromString(TEXT("Coupe"));

	// Mass and dimensions: 1300 kg, agile, low drag
	Data->MassKg = 1300.0f;
	Data->CenterOfMassOffset = FVector(0.0f, 0.0f, -15.0f);
	Data->DragCoefficient = 0.32f; // Lower drag for sports car

	// Drivetrain: RWD, rotation-friendly
	Data->DrivetrainType = EMZDrivetrainType::RWD;
	Data->GearRatios.Empty();
	Data->GearRatios.Add(3.8f);
	Data->GearRatios.Add(2.2f);
	Data->GearRatios.Add(1.5f);
	Data->GearRatios.Add(1.1f);
	Data->GearRatios.Add(0.85f);
	Data->GearRatios.Add(0.68f);
	Data->FinalDriveRatio = 3.7f;

	// Steering: agile response
	Data->MaxSteerAngle = 32.0f;
	Data->SteeringSpeed = 5.0f;

	// Engine: 400 Nm, 7000 RPM high-revving
	Data->Engine.MaxTorque = 400.0f;
	Data->Engine.MaxRPM = 7000.0f;
	Data->Engine.IdleRPM = 900.0f;
	Data->Engine.EngineRevDownRate = 2000.0f;

	// Brakes: firm, with good front bias for weight transfer
	Data->Brakes.MaxBrakeTorque = 4000.0f;
	Data->Brakes.HandbrakeTorque = 6500.0f;
	Data->Brakes.FrontBrakeBias = 0.65f;

	// Suspension: stiff and responsive (short wheelbase feel)
	// Front wheels: stiffer springs for responsive turn-in
	Data->FrontWheels.SpringRate = 32000.0f;
	Data->FrontWheels.DampingRateBump = 3200.0f;
	Data->FrontWheels.DampingRateRebound = 4800.0f;
	Data->FrontWheels.SuspensionMaxDrop = 10.0f;
	Data->FrontWheels.SuspensionMaxRaise = 10.0f;
	Data->FrontWheels.WheelRadius = 35.0f;
	Data->FrontWheels.WheelWidth = 22.0f;
	Data->FrontWheels.LateralStiffness = 1200.0f;  // Higher lateral stiffness for cornering grip
	Data->FrontWheels.LongitudinalStiffness = 1100.0f;

	// Rear wheels: also stiff but slightly softer than front to maintain balance
	Data->RearWheels.SpringRate = 30000.0f;
	Data->RearWheels.DampingRateBump = 3000.0f;
	Data->RearWheels.DampingRateRebound = 4500.0f;
	Data->RearWheels.SuspensionMaxDrop = 10.0f;
	Data->RearWheels.SuspensionMaxRaise = 10.0f;
	Data->RearWheels.WheelRadius = 35.0f;
	Data->RearWheels.WheelWidth = 22.0f;
	Data->RearWheels.LateralStiffness = 1150.0f;
	Data->RearWheels.LongitudinalStiffness = 1050.0f;

	UE_LOG(LogMileZero, Log, TEXT("Created Coupe data: 1300 kg, RWD, 400 Nm, 7000 RPM"));
	return Data;
}

UMZVehicleDataAsset* UMZVehicleRegistry::CreateHatchData()
{
	UMZVehicleDataAsset* Data = NewObject<UMZVehicleDataAsset>(this, FName("DA_Hatch"));
	if (!Data)
	{
		return nullptr;
	}

	// Identity
	Data->VehicleID = FName("Hatch");
	Data->DisplayName = FText::FromString(TEXT("Hatchback"));

	// Mass and dimensions: 1150 kg, practical, moderate drag
	Data->MassKg = 1150.0f;
	Data->CenterOfMassOffset = FVector(0.0f, 0.0f, -12.0f); // Slightly higher CoM for comfort
	Data->DragCoefficient = 0.35f; // Moderate drag

	// Drivetrain: FWD, stable and forgiving
	Data->DrivetrainType = EMZDrivetrainType::FWD;
	Data->GearRatios.Empty();
	Data->GearRatios.Add(3.5f);
	Data->GearRatios.Add(2.0f);
	Data->GearRatios.Add(1.35f);
	Data->GearRatios.Add(1.0f);
	Data->GearRatios.Add(0.82f);
	Data->FinalDriveRatio = 3.9f;

	// Steering: responsive but safe
	Data->MaxSteerAngle = 38.0f;
	Data->SteeringSpeed = 5.0f;

	// Engine: 280 Nm, 6500 RPM, moderate power
	Data->Engine.MaxTorque = 280.0f;
	Data->Engine.MaxRPM = 6500.0f;
	Data->Engine.IdleRPM = 900.0f;
	Data->Engine.EngineRevDownRate = 1800.0f;

	// Brakes: balanced, comfortable deceleration
	Data->Brakes.MaxBrakeTorque = 3500.0f;
	Data->Brakes.HandbrakeTorque = 5500.0f;
	Data->Brakes.FrontBrakeBias = 0.60f;

	// Suspension: moderate compliance, forgiving
	// Front wheels: medium springs for comfort and control
	Data->FrontWheels.SpringRate = 25000.0f;
	Data->FrontWheels.DampingRateBump = 2800.0f;
	Data->FrontWheels.DampingRateRebound = 4200.0f;
	Data->FrontWheels.SuspensionMaxDrop = 11.0f;
	Data->FrontWheels.SuspensionMaxRaise = 11.0f;
	Data->FrontWheels.WheelRadius = 35.0f;
	Data->FrontWheels.WheelWidth = 22.0f;
	Data->FrontWheels.LateralStiffness = 1000.0f;  // Moderate cornering grip
	Data->FrontWheels.LongitudinalStiffness = 1000.0f;

	// Rear wheels: slightly softer for mild understeer tendency (FWD trait)
	Data->RearWheels.SpringRate = 24000.0f;
	Data->RearWheels.DampingRateBump = 2700.0f;
	Data->RearWheels.DampingRateRebound = 4000.0f;
	Data->RearWheels.SuspensionMaxDrop = 11.0f;
	Data->RearWheels.SuspensionMaxRaise = 11.0f;
	Data->RearWheels.WheelRadius = 35.0f;
	Data->RearWheels.WheelWidth = 22.0f;
	Data->RearWheels.LateralStiffness = 950.0f;
	Data->RearWheels.LongitudinalStiffness = 950.0f;

	UE_LOG(LogMileZero, Log, TEXT("Created Hatchback data: 1150 kg, FWD, 280 Nm, 6500 RPM"));
	return Data;
}

UMZVehicleDataAsset* UMZVehicleRegistry::CreatePickupData()
{
	UMZVehicleDataAsset* Data = NewObject<UMZVehicleDataAsset>(this, FName("DA_Pickup"));
	if (!Data)
	{
		return nullptr;
	}

	// Identity
	Data->VehicleID = FName("Pickup");
	Data->DisplayName = FText::FromString(TEXT("Pickup/SUV"));

	// Mass and dimensions: 2000 kg, heavy with high CoM
	Data->MassKg = 2000.0f;
	Data->CenterOfMassOffset = FVector(0.0f, 0.0f, -20.0f); // Higher CoM for SUV-like feel
	Data->DragCoefficient = 0.42f; // Higher drag, poor aerodynamics

	// Drivetrain: AWD, capable but ponderous
	Data->DrivetrainType = EMZDrivetrainType::AWD;
	Data->GearRatios.Empty();
	Data->GearRatios.Add(4.0f);
	Data->GearRatios.Add(2.3f);
	Data->GearRatios.Add(1.5f);
	Data->GearRatios.Add(1.0f);
	Data->GearRatios.Add(0.75f);
	Data->FinalDriveRatio = 4.1f;

	// Steering: slower, more truck-like
	Data->MaxSteerAngle = 30.0f;
	Data->SteeringSpeed = 5.0f;

	// Engine: 500 Nm, 5500 RPM, torquey but not high-revving
	Data->Engine.MaxTorque = 500.0f;
	Data->Engine.MaxRPM = 5500.0f;
	Data->Engine.IdleRPM = 900.0f;
	Data->Engine.EngineRevDownRate = 1500.0f;

	// Brakes: strong, higher bias to rear for load-carrying
	Data->Brakes.MaxBrakeTorque = 4500.0f;
	Data->Brakes.HandbrakeTorque = 7000.0f;
	Data->Brakes.FrontBrakeBias = 0.55f; // More rear bias for truck feel

	// Suspension: soft, compliant, body roll is expected
	// Front wheels: soft springs for load absorption and comfort
	Data->FrontWheels.SpringRate = 18000.0f;
	Data->FrontWheels.DampingRateBump = 2400.0f;
	Data->FrontWheels.DampingRateRebound = 3600.0f;
	Data->FrontWheels.SuspensionMaxDrop = 13.0f;
	Data->FrontWheels.SuspensionMaxRaise = 13.0f;
	Data->FrontWheels.WheelRadius = 37.0f;  // Larger wheels for SUV
	Data->FrontWheels.WheelWidth = 24.0f;   // Wider track
	Data->FrontWheels.LateralStiffness = 800.0f;   // Lower cornering grip (poor handling)
	Data->FrontWheels.LongitudinalStiffness = 850.0f;

	// Rear wheels: equally soft, maintains stability
	Data->RearWheels.SpringRate = 17500.0f;
	Data->RearWheels.DampingRateBump = 2300.0f;
	Data->RearWheels.DampingRateRebound = 3500.0f;
	Data->RearWheels.SuspensionMaxDrop = 13.0f;
	Data->RearWheels.SuspensionMaxRaise = 13.0f;
	Data->RearWheels.WheelRadius = 37.0f;
	Data->RearWheels.WheelWidth = 24.0f;
	Data->RearWheels.LateralStiffness = 800.0f;
	Data->RearWheels.LongitudinalStiffness = 850.0f;

	UE_LOG(LogMileZero, Log, TEXT("Created Pickup/SUV data: 2000 kg, AWD, 500 Nm, 5500 RPM"));
	return Data;
}
