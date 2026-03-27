#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MZDamageTypes.h"
#include "MZVehicleDamageComponent.generated.h"

class UMZDamageDataAsset;

/**
 * Vehicle damage component that tracks and applies mechanical damage
 * Manages steering pull, engine power loss, suspension degradation, aerodynamic drag,
 * brake effectiveness, cooling system, and wheel-specific traction loss.
 *
 * All damage is normalized 0-1 and applied through multiplier functions that the
 * vehicle (or camera) systems can query to apply effects in real-time.
 */
UCLASS(ClassGroup = "MileZero", meta = (BlueprintSpawnableComponent))
class MILEZERO_API UMZVehicleDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMZVehicleDamageComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- Configuration ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Damage")
	TObjectPtr<UMZDamageDataAsset> DamageConfig;

	// --- Core Damage State (all 0-1 normalized) ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|State")
	float SteeringDamage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|State")
	float EngineDamage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|State")
	float CoolingDamage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|State")
	float AeroDamage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|State")
	float BrakeDamage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|State")
	TArray<float> SuspensionDamage = { 0.0f, 0.0f, 0.0f, 0.0f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|State")
	TArray<float> WheelDamage = { 0.0f, 0.0f, 0.0f, 0.0f };

	// --- Overheat State ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|Overheat")
	bool bIsOverheating = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Damage|Overheat")
	float OverheatLevel = 0.0f;

	// --- Public API ---

	/**
	 * Process an impact collision
	 * Determines severity from speed and distributes damage to subsystems based on impact geometry
	 *
	 * @param ImpactSpeedKmh Speed magnitude of the collision
	 * @param ImpactNormal World-space surface normal (direction vehicle hit)
	 * @param ImpactPoint World location of impact
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Damage")
	void ProcessImpact(float ImpactSpeedKmh, FVector ImpactNormal, FVector ImpactPoint);

	/**
	 * Get steering pull caused by damage
	 * @return Steering pull in degrees (positive = right, negative = left)
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetSteeringPullDegrees() const;

	/**
	 * Get power output multiplier
	 * @return 1.0 = full power, <1.0 = reduced power due to engine damage
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetPowerMultiplier() const;

	/**
	 * Get aerodynamic drag multiplier
	 * @return 1.0 = baseline drag, >1.0 = increased drag due to damage
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetDragMultiplier() const;

	/**
	 * Get brake effectiveness multiplier
	 * @return 1.0 = full braking, <1.0 = reduced brake force due to damage
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetBrakeMultiplier() const;

	/**
	 * Get suspension effectiveness for a specific wheel
	 * @param WheelIndex 0=FL, 1=FR, 2=RL, 3=RR
	 * @return 1.0 = full damping, <1.0 = reduced suspension effectiveness
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetSuspensionMultiplier(int32 WheelIndex) const;

	/**
	 * Get grip/traction multiplier for a specific wheel
	 * @param WheelIndex 0=FL, 1=FR, 2=RL, 3=RR
	 * @return 1.0 = full grip, <1.0 = reduced traction due to damage
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetWheelGripMultiplier(int32 WheelIndex) const;

	/**
	 * Get overheat progression factor (0-1)
	 * Returns 0 if not overheating, ramps to 1 over time when cooling is damaged
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetOverheatFactor() const;

	/**
	 * Get overall damage as a percentage (0-100)
	 * Weighted average of all damage systems
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage|Effects")
	float GetOverallDamagePercent() const;

	/**
	 * Reset all damage to zero
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Damage")
	void RepairAll();

	/**
	 * Get current damage state as a serializable snapshot
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Damage")
	FMZDamageState GetDamageState() const;

	/**
	 * Restore damage from a saved snapshot
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Damage")
	void RestoreDamageState(const FMZDamageState& State);

private:
	// Overheat system
	float OverheatTimer = 0.0f;
	static constexpr float OVERHEAT_RAMP_TIME = 15.0f; // Time to reach full overheat
	static constexpr float COOLING_THRESHOLD = 0.3f; // CoolingDamage threshold to start overheat

	// Impact processing helpers
	void ApplyDamageToSubsystems(
		float DamageAmount,
		const FVector& ImpactNormal,
		const FVector& VehicleForward
	);

	EMZImpactSeverity CalculateSeverity(float ImpactSpeedKmh) const;

	// Clamp normalized damage to 0-1
	float ClampDamage(float Value) const
	{
		return FMath::Clamp(Value, 0.0f, 1.0f);
	}
};
