#include "Damage/MZVehicleDamageComponent.h"
#include "Damage/MZDamageDataAsset.h"
#include "MileZero.h"

UMZVehicleDamageComponent::UMZVehicleDamageComponent()
{
	PrimaryComponentTick.TickInterval = 0.0f;
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize all damage to zero
	SteeringDamage = 0.0f;
	EngineDamage = 0.0f;
	CoolingDamage = 0.0f;
	AeroDamage = 0.0f;
	BrakeDamage = 0.0f;
	bIsOverheating = false;
	OverheatLevel = 0.0f;

	for (int32 i = 0; i < 4; ++i)
	{
		SuspensionDamage[i] = 0.0f;
		WheelDamage[i] = 0.0f;
	}
}

void UMZVehicleDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	// Use default config if none assigned
	if (!DamageConfig)
	{
		DamageConfig = NewObject<UMZDamageDataAsset>();
		if (DamageConfig)
		{
			DamageConfig->DamageProfileID = FName("RuntimeDefault");
			UE_LOG(LogMileZero, Warning, TEXT("MZVehicleDamageComponent: No DamageConfig assigned, using runtime defaults"));
		}
	}
}

void UMZVehicleDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!DamageConfig)
	{
		return;
	}

	// --- Auto-repair if enabled ---
	if (DamageConfig->RepairRate > 0.0f)
	{
		float RepairAmount = DamageConfig->RepairRate * DeltaTime;

		SteeringDamage = FMath::Max(0.0f, SteeringDamage - RepairAmount);
		EngineDamage = FMath::Max(0.0f, EngineDamage - RepairAmount);
		CoolingDamage = FMath::Max(0.0f, CoolingDamage - RepairAmount);
		AeroDamage = FMath::Max(0.0f, AeroDamage - RepairAmount);
		BrakeDamage = FMath::Max(0.0f, BrakeDamage - RepairAmount);

		for (int32 i = 0; i < 4; ++i)
		{
			SuspensionDamage[i] = FMath::Max(0.0f, SuspensionDamage[i] - RepairAmount);
			WheelDamage[i] = FMath::Max(0.0f, WheelDamage[i] - RepairAmount);
		}
	}

	// --- Overheating system ---
	if (CoolingDamage > COOLING_THRESHOLD)
	{
		// Check if throttle is being applied (simplified: assume any damage + time = overheat)
		bIsOverheating = true;
		OverheatTimer += DeltaTime;
		OverheatLevel = FMath::Min(1.0f, OverheatTimer / OVERHEAT_RAMP_TIME);

		if (OverheatLevel >= 1.0f)
		{
			UE_LOG(LogMileZero, Warning, TEXT("Vehicle is OVERHEATING (CoolingDamage=%.2f, OverheatLevel=%.2f)"), CoolingDamage, OverheatLevel);
		}
	}
	else
	{
		// Cool down
		if (bIsOverheating)
		{
			OverheatTimer = FMath::Max(0.0f, OverheatTimer - DeltaTime * 0.5f); // Cool faster than heating
			OverheatLevel = FMath::Max(0.0f, OverheatLevel - DeltaTime * 0.5f);

			if (OverheatLevel <= 0.0f)
			{
				bIsOverheating = false;
			}
		}
	}
}

void UMZVehicleDamageComponent::ProcessImpact(float ImpactSpeedKmh, FVector ImpactNormal, FVector ImpactPoint)
{
	if (!DamageConfig)
	{
		return;
	}

	// Normalize impact normal
	ImpactNormal.Normalize();

	// Calculate severity
	EMZImpactSeverity Severity = CalculateSeverity(ImpactSpeedKmh);
	if (Severity == EMZImpactSeverity::None)
	{
		return; // No damage below threshold
	}

	// Scale damage amount based on severity and speed
	float DamageAmount = 0.0f;
	if (Severity == EMZImpactSeverity::Light)
	{
		DamageAmount = FMath::Clamp(
			(ImpactSpeedKmh - DamageConfig->LightImpactSpeed) / (DamageConfig->MediumImpactSpeed - DamageConfig->LightImpactSpeed),
			0.05f, 0.15f
		);
	}
	else if (Severity == EMZImpactSeverity::Medium)
	{
		DamageAmount = FMath::Clamp(
			(ImpactSpeedKmh - DamageConfig->MediumImpactSpeed) / (DamageConfig->HeavyImpactSpeed - DamageConfig->MediumImpactSpeed),
			0.2f, 0.4f
		);
	}
	else // Heavy
	{
		DamageAmount = FMath::Clamp((ImpactSpeedKmh - DamageConfig->HeavyImpactSpeed) / 100.0f, 0.4f, 1.0f);
	}

	// Get vehicle forward direction for impact classification
	AActor* Owner = GetOwner();
	FVector VehicleForward = Owner ? Owner->GetActorForwardVector() : FVector::ForwardVector;

	// Apply to subsystems based on impact direction
	ApplyDamageToSubsystems(DamageAmount, ImpactNormal, VehicleForward);

	UE_LOG(LogMileZero, Log, TEXT("Impact: Speed=%.1f km/h, Severity=%d, Damage=%.2f"), ImpactSpeedKmh, (int32)Severity, DamageAmount);
}

EMZImpactSeverity UMZVehicleDamageComponent::CalculateSeverity(float ImpactSpeedKmh) const
{
	if (!DamageConfig)
	{
		return EMZImpactSeverity::None;
	}

	if (ImpactSpeedKmh < DamageConfig->LightImpactSpeed)
	{
		return EMZImpactSeverity::None;
	}
	else if (ImpactSpeedKmh < DamageConfig->MediumImpactSpeed)
	{
		return EMZImpactSeverity::Light;
	}
	else if (ImpactSpeedKmh < DamageConfig->HeavyImpactSpeed)
	{
		return EMZImpactSeverity::Medium;
	}

	return EMZImpactSeverity::Heavy;
}

void UMZVehicleDamageComponent::ApplyDamageToSubsystems(
	float DamageAmount,
	const FVector& ImpactNormal,
	const FVector& VehicleForward
)
{
	// Determine impact direction
	// Forward vector dot product tells us if we hit front, rear, or side
	float ForwardDot = FVector::DotProduct(ImpactNormal, VehicleForward);
	float SideDot = FVector::DotProduct(ImpactNormal, VehicleForward.RotateAngleAxis(90.0f, FVector::UpVector));

	// Add some randomness for variety (±20%)
	float RandomFactor = FMath::RandRange(0.8f, 1.2f);
	DamageAmount *= RandomFactor;
	DamageAmount = ClampDamage(DamageAmount);

	// --- FRONT IMPACT (ForwardDot > 0.5) ---
	if (ForwardDot > 0.5f)
	{
		// Engine, cooling, front suspension, aero
		EngineDamage = ClampDamage(EngineDamage + DamageAmount * 0.9f);
		CoolingDamage = ClampDamage(CoolingDamage + DamageAmount * 0.8f);
		AeroDamage = ClampDamage(AeroDamage + DamageAmount * 0.7f);

		// Front wheels (FL=0, FR=1)
		SuspensionDamage[0] = ClampDamage(SuspensionDamage[0] + DamageAmount * 0.8f);
		SuspensionDamage[1] = ClampDamage(SuspensionDamage[1] + DamageAmount * 0.8f);
		WheelDamage[0] = ClampDamage(WheelDamage[0] + DamageAmount * 0.6f);
		WheelDamage[1] = ClampDamage(WheelDamage[1] + DamageAmount * 0.6f);
	}
	// --- REAR IMPACT (ForwardDot < -0.5) ---
	else if (ForwardDot < -0.5f)
	{
		// Rear suspension and aero
		AeroDamage = ClampDamage(AeroDamage + DamageAmount * 0.8f);

		// Rear wheels (RL=2, RR=3)
		SuspensionDamage[2] = ClampDamage(SuspensionDamage[2] + DamageAmount * 0.85f);
		SuspensionDamage[3] = ClampDamage(SuspensionDamage[3] + DamageAmount * 0.85f);
		WheelDamage[2] = ClampDamage(WheelDamage[2] + DamageAmount * 0.7f);
		WheelDamage[3] = ClampDamage(WheelDamage[3] + DamageAmount * 0.7f);
	}
	// --- SIDE IMPACT ---
	else
	{
		// Steering and side wheels + suspension
		SteeringDamage = ClampDamage(SteeringDamage + DamageAmount * 0.95f);

		// Determine which side (left < 0, right > 0)
		if (SideDot > 0.0f)
		{
			// Right side impact (FR=1, RR=3)
			SuspensionDamage[1] = ClampDamage(SuspensionDamage[1] + DamageAmount * 0.85f);
			SuspensionDamage[3] = ClampDamage(SuspensionDamage[3] + DamageAmount * 0.85f);
			WheelDamage[1] = ClampDamage(WheelDamage[1] + DamageAmount * 0.75f);
			WheelDamage[3] = ClampDamage(WheelDamage[3] + DamageAmount * 0.75f);
		}
		else
		{
			// Left side impact (FL=0, RL=2)
			SuspensionDamage[0] = ClampDamage(SuspensionDamage[0] + DamageAmount * 0.85f);
			SuspensionDamage[2] = ClampDamage(SuspensionDamage[2] + DamageAmount * 0.85f);
			WheelDamage[0] = ClampDamage(WheelDamage[0] + DamageAmount * 0.75f);
			WheelDamage[2] = ClampDamage(WheelDamage[2] + DamageAmount * 0.75f);
		}

		// Some brake damage from side impacts
		BrakeDamage = ClampDamage(BrakeDamage + DamageAmount * 0.3f);
	}
}

float UMZVehicleDamageComponent::GetSteeringPullDegrees() const
{
	if (!DamageConfig)
	{
		return 0.0f;
	}

	// Asymmetric damage (one side vs other) causes pull
	float LeftDamage = (SuspensionDamage[0] + WheelDamage[0]) * 0.5f;
	float RightDamage = (SuspensionDamage[1] + WheelDamage[1]) * 0.5f;
	float DamageDifference = (RightDamage - LeftDamage) + SteeringDamage * 0.5f;

	return DamageDifference * DamageConfig->MaxSteeringPull;
}

float UMZVehicleDamageComponent::GetPowerMultiplier() const
{
	if (!DamageConfig)
	{
		return 1.0f;
	}

	return 1.0f - (EngineDamage * DamageConfig->MaxPowerLoss);
}

float UMZVehicleDamageComponent::GetDragMultiplier() const
{
	if (!DamageConfig)
	{
		return 1.0f;
	}

	return 1.0f + (AeroDamage * DamageConfig->MaxDragIncrease);
}

float UMZVehicleDamageComponent::GetBrakeMultiplier() const
{
	if (!DamageConfig)
	{
		return 1.0f;
	}

	return 1.0f - (BrakeDamage * DamageConfig->MaxBrakeDegrade);
}

float UMZVehicleDamageComponent::GetSuspensionMultiplier(int32 WheelIndex) const
{
	if (!DamageConfig || WheelIndex < 0 || WheelIndex > 3)
	{
		return 1.0f;
	}

	return 1.0f - (SuspensionDamage[WheelIndex] * DamageConfig->MaxSuspensionDegrade);
}

float UMZVehicleDamageComponent::GetWheelGripMultiplier(int32 WheelIndex) const
{
	if (WheelIndex < 0 || WheelIndex > 3)
	{
		return 1.0f;
	}

	// Wheel damage directly reduces grip
	return 1.0f - WheelDamage[WheelIndex];
}

float UMZVehicleDamageComponent::GetOverheatFactor() const
{
	return bIsOverheating ? OverheatLevel : 0.0f;
}

float UMZVehicleDamageComponent::GetOverallDamagePercent() const
{
	// Weighted average of all damage systems
	float Total = SteeringDamage * 0.15f;
	Total += EngineDamage * 0.25f;
	Total += CoolingDamage * 0.15f;
	Total += AeroDamage * 0.10f;
	Total += BrakeDamage * 0.15f;

	// Include suspension and wheel damage
	for (int32 i = 0; i < 4; ++i)
	{
		Total += SuspensionDamage[i] * 0.025f; // 0.1 total / 4 wheels
		Total += WheelDamage[i] * 0.025f;
	}

	return FMath::Clamp(Total, 0.0f, 1.0f) * 100.0f; // Return as percentage
}

void UMZVehicleDamageComponent::RepairAll()
{
	if (DamageConfig && !DamageConfig->bCanReset)
	{
		UE_LOG(LogMileZero, Warning, TEXT("MZVehicleDamageComponent: RepairAll() called but bCanReset=false"));
		return;
	}

	SteeringDamage = 0.0f;
	EngineDamage = 0.0f;
	CoolingDamage = 0.0f;
	AeroDamage = 0.0f;
	BrakeDamage = 0.0f;
	bIsOverheating = false;
	OverheatTimer = 0.0f;
	OverheatLevel = 0.0f;

	for (int32 i = 0; i < 4; ++i)
	{
		SuspensionDamage[i] = 0.0f;
		WheelDamage[i] = 0.0f;
	}

	UE_LOG(LogMileZero, Log, TEXT("All vehicle damage repaired"));
}

FMZDamageState UMZVehicleDamageComponent::GetDamageState() const
{
	FMZDamageState State;

	State.SteeringDamage = SteeringDamage;
	State.EngineDamage = EngineDamage;
	State.CoolingDamage = CoolingDamage;
	State.AeroDamage = AeroDamage;
	State.BrakeDamage = BrakeDamage;

	for (int32 i = 0; i < 4; ++i)
	{
		State.SuspensionDamage[i] = SuspensionDamage[i];
		State.WheelDamage[i] = WheelDamage[i];
	}

	return State;
}

void UMZVehicleDamageComponent::RestoreDamageState(const FMZDamageState& State)
{
	SteeringDamage = State.SteeringDamage;
	EngineDamage = State.EngineDamage;
	CoolingDamage = State.CoolingDamage;
	AeroDamage = State.AeroDamage;
	BrakeDamage = State.BrakeDamage;

	for (int32 i = 0; i < 4; ++i)
	{
		SuspensionDamage[i] = State.SuspensionDamage[i];
		WheelDamage[i] = State.WheelDamage[i];
	}

	bIsOverheating = CoolingDamage > COOLING_THRESHOLD;
	OverheatTimer = OverheatLevel * OVERHEAT_RAMP_TIME;

	UE_LOG(LogMileZero, Log, TEXT("Damage state restored: Overall=%.1f%%"), GetOverallDamagePercent());
}
