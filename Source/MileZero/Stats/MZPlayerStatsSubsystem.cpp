#include "MZPlayerStatsSubsystem.h"
#include "Vehicles/MZVehiclePawn.h"
#include "MileZero.h"

DEFINE_LOG_CATEGORY_STATIC(LogMZStats, Log, All);

void UMZPlayerStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Stats = FMZPlayerStats();
	CurrentVehicle = nullptr;
	CurrentVehicleID = FName(TEXT("None"));

	UE_LOG(LogMileZero, Log, TEXT("UMZPlayerStatsSubsystem initialized"));
}

void UMZPlayerStatsSubsystem::Deinitialize()
{
	Super::Deinitialize();
	CurrentVehicle = nullptr;
	UE_LOG(LogMileZero, Log, TEXT("UMZPlayerStatsSubsystem deinitialized"));
}

void UMZPlayerStatsSubsystem::Tick(float DeltaTime)
{
	// Only update if we have a vehicle to track
	if (CurrentVehicle && !CurrentVehicle->IsActorBeingDestroyed())
	{
		UpdateDistanceAndSpeed(DeltaTime);

		// Periodically recalculate skill
		LastSkillRecalcTime += DeltaTime;
		if (LastSkillRecalcTime >= SKILL_RECALC_INTERVAL)
		{
			RecalculateDrivingSkill();
			LastSkillRecalcTime = 0.0f;
		}
	}

	// Always accumulate playtime
	Stats.TotalPlayTime += DeltaTime;
}

void UMZPlayerStatsSubsystem::OnVehiclePossessed(AMZVehiclePawn* Vehicle)
{
	if (!Vehicle)
	{
		return;
	}

	CurrentVehicle = Vehicle;

	// Get vehicle ID from the vehicle pawn
	// For now, use a basic naming scheme; real vehicles would have formal IDs
	FString VehicleClass = Vehicle->GetClass()->GetName();
	CurrentVehicleID = FName(*VehicleClass);

	if (!Stats.VehicleUsageCount.Contains(CurrentVehicleID))
	{
		Stats.VehicleUsageCount.Add(CurrentVehicleID, 0);
	}
	Stats.VehicleUsageCount[CurrentVehicleID]++;

	if (!Stats.VehicleDistances.Contains(CurrentVehicleID))
	{
		Stats.VehicleDistances.Add(CurrentVehicleID, 0.0f);
	}

	UE_LOG(LogMileZero, Log, TEXT("Vehicle possessed: %s (usage count: %d)"),
		*CurrentVehicleID.ToString(), Stats.VehicleUsageCount[CurrentVehicleID]);
}

void UMZPlayerStatsSubsystem::OnVehicleUnpossessed()
{
	if (CurrentVehicle)
	{
		UE_LOG(LogMileZero, Log, TEXT("Vehicle unpossessed: %s"), *CurrentVehicleID.ToString());
	}
	CurrentVehicle = nullptr;
}

void UMZPlayerStatsSubsystem::OnActivityCompleted(const FName& ActivityID, bool bSuccess)
{
	if (bSuccess)
	{
		Stats.TotalActivitiesCompleted++;
		UE_LOG(LogMileZero, Log, TEXT("Activity completed: %s (total: %d)"),
			*ActivityID.ToString(), Stats.TotalActivitiesCompleted);
	}
	else
	{
		Stats.TotalActivitiesFailed++;
		UE_LOG(LogMileZero, Log, TEXT("Activity failed: %s (total: %d)"),
			*ActivityID.ToString(), Stats.TotalActivitiesFailed);
	}

	RecalculateDrivingSkill();
}

void UMZPlayerStatsSubsystem::OnCrash(float DamageApplied)
{
	Stats.TotalCrashes++;
	UE_LOG(LogMileZero, Log, TEXT("Crash recorded (damage: %.2f, total crashes: %d)"),
		DamageApplied, Stats.TotalCrashes);

	RecalculateDrivingSkill();
}

FText UMZPlayerStatsSubsystem::GetStatsSummary() const
{
	FString Summary = FString::Printf(
		TEXT("=== PLAYER STATISTICS ===\n")
		TEXT("Distance Driven: %.1f km\n")
		TEXT("Play Time: %.0f min\n")
		TEXT("Activities Completed: %d\n")
		TEXT("Activities Failed: %d\n")
		TEXT("Total Crashes: %d\n")
		TEXT("Top Speed: %.1f km/h\n")
		TEXT("Driving Skill: %.0f%%"),
		Stats.TotalDistanceDriven,
		Stats.TotalPlayTime / 60.0f,
		Stats.TotalActivitiesCompleted,
		Stats.TotalActivitiesFailed,
		Stats.TotalCrashes,
		Stats.TopSpeedEverKmh,
		Stats.DrivingSkill
	);

	return FText::FromString(Summary);
}

void UMZPlayerStatsSubsystem::ResetStats()
{
	Stats = FMZPlayerStats();
	CurrentVehicle = nullptr;
	CurrentVehicleID = FName(TEXT("None"));
	LastSkillRecalcTime = 0.0f;
	LastRecordedSpeedKmh = 0.0f;

	UE_LOG(LogMileZero, Log, TEXT("Player stats reset"));
}

void UMZPlayerStatsSubsystem::UpdateDistanceAndSpeed(float DeltaTime)
{
	if (!CurrentVehicle)
	{
		return;
	}

	// Get current speed from vehicle
	float CurrentSpeedKmh = CurrentVehicle->GetSpeedKmh();

	// Update distance traveled (convert to km)
	// Assumption: vehicle velocity is in cm/s, so (speed * DeltaTime / 100000) gives km
	float DistanceThisFrame = (CurrentSpeedKmh * DeltaTime) / 3600.0f; // km/h * s / 3600 = km
	Stats.TotalDistanceDriven += DistanceThisFrame;

	// Update per-vehicle distance
	if (Stats.VehicleDistances.Contains(CurrentVehicleID))
	{
		Stats.VehicleDistances[CurrentVehicleID] += DistanceThisFrame;
	}

	// Track top speed
	if (CurrentSpeedKmh > Stats.TopSpeedEverKmh)
	{
		Stats.TopSpeedEverKmh = CurrentSpeedKmh;
	}
}

void UMZPlayerStatsSubsystem::RecalculateDrivingSkill()
{
	// Skill calculation based on:
	// - Activities completed (positive)
	// - Activities failed (negative)
	// - Total crashes (negative)
	// Base starts at 50, ranges 0-100

	int32 CompletedCount = Stats.TotalActivitiesCompleted;
	int32 FailedCount = Stats.TotalActivitiesFailed;
	int32 CrashCount = Stats.TotalCrashes;

	float SkillDelta = 0.0f;

	// Each completed activity: +2 skill
	SkillDelta += CompletedCount * 2.0f;

	// Each failed activity: -1 skill
	SkillDelta -= FailedCount * 1.0f;

	// Each crash: -0.5 skill
	SkillDelta -= CrashCount * 0.5f;

	Stats.DrivingSkill = FMath::Clamp(50.0f + SkillDelta, 0.0f, 100.0f);
}
