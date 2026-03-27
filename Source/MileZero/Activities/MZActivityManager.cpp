#include "MZActivityManager.h"
#include "MZCheckpointActor.h"
#include "MZDeliveryPickup.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "MileZero.h"

void UMZActivityManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsTickEnabled = true;
	UE_LOG(LogMileZero, Log, TEXT("Activity Manager initialized"));
}

void UMZActivityManager::Deinitialize()
{
	ClearActivityState();
	Super::Deinitialize();
}

void UMZActivityManager::Tick(float DeltaTime)
{
	if (ActivityState == EMZActivityState::Active && bIsTickEnabled)
	{
		TickActivity(DeltaTime);
	}
}

void UMZActivityManager::StartActivity(UMZActivityDataAsset* Activity)
{
	if (!Activity)
	{
		UE_LOG(LogMileZero, Warning, TEXT("StartActivity called with null activity"));
		return;
	}

	// Clear any previous activity
	ClearActivityState();

	CurrentActivity = Activity;
	ActivityState = EMZActivityState::Starting;
	ElapsedTime = 0.0f;
	AccumulatedDamage = 0.0f;
	bCargoPickedUp = false;
	TrafficVehiclesPassed = 0;

	UE_LOG(LogMileZero, Log, TEXT("Starting activity: %s (Type: %d)"),
		*Activity->ActivityID.ToString(), (int32)Activity->ActivityType);

	// Setup based on activity type
	switch (Activity->ActivityType)
	{
	case EMZActivityType::CheckpointSprint:
		SetupCheckpointSprint();
		break;
	case EMZActivityType::DeliveryRun:
		SetupDeliveryRun();
		break;
	case EMZActivityType::TrafficWeave:
		SetupTrafficWeave();
		break;
	case EMZActivityType::DamageSurvival:
		SetupDamageSurvival();
		break;
	default:
		UE_LOG(LogMileZero, Warning, TEXT("Unknown activity type"));
		return;
	}

	// Transition to active
	ActivityState = EMZActivityState::Active;
	OnActivityStateChanged.Broadcast(ActivityState, FText::FromString("Activity started"), true);
	BroadcastProgressUpdate();
}

void UMZActivityManager::AbandonActivity()
{
	if (ActivityState == EMZActivityState::Inactive)
	{
		return;
	}

	FailActivity(TEXT("Activity abandoned"));
}

void UMZActivityManager::CompleteActivity()
{
	if (ActivityState != EMZActivityState::Active)
	{
		return;
	}

	ActivityState = EMZActivityState::Completed;

	// Record best time
	if (CurrentActivity && CurrentActivity->TimeLimitSeconds > 0.0f)
	{
		float& BestTime = BestTimes.FindOrAdd(CurrentActivity->ActivityID);
		if (BestTime == 0.0f || ElapsedTime < BestTime)
		{
			BestTime = ElapsedTime;
			UE_LOG(LogMileZero, Log, TEXT("New best time for %s: %.2f seconds"),
				*CurrentActivity->ActivityID.ToString(), ElapsedTime);
		}
	}

	FText CompleteMsg = FText::FromString(FString::Printf(TEXT("Activity completed in %.2f seconds"), ElapsedTime));
	OnActivityStateChanged.Broadcast(ActivityState, CompleteMsg, true);
	BroadcastProgressUpdate();

	UE_LOG(LogMileZero, Log, TEXT("Activity completed: %s"), *CurrentActivity->ActivityID.ToString());
}

void UMZActivityManager::FailActivity(const FString& Reason)
{
	if (ActivityState == EMZActivityState::Inactive || ActivityState == EMZActivityState::Failed)
	{
		return;
	}

	ActivityState = EMZActivityState::Failed;
	ClearActivityState();

	FText FailMsg = FText::FromString(FString::Printf(TEXT("Activity failed: %s"), *Reason));
	OnActivityStateChanged.Broadcast(ActivityState, FailMsg, false);

	UE_LOG(LogMileZero, Log, TEXT("Activity failed: %s (Reason: %s)"),
		CurrentActivity ? *CurrentActivity->ActivityID.ToString() : TEXT("Unknown"), *Reason);
}

void UMZActivityManager::OnCheckpointReached(int32 CheckpointIndex, bool bIsFinish)
{
	if (ActivityState != EMZActivityState::Active)
	{
		return;
	}

	if (CurrentActivity && CurrentActivity->ActivityType != EMZActivityType::CheckpointSprint)
	{
		return;
	}

	UE_LOG(LogMileZero, Log, TEXT("Checkpoint %d reached (Finish: %s)"),
		CheckpointIndex, bIsFinish ? TEXT("true") : TEXT("false"));

	CurrentCheckpointIndex = CheckpointIndex;

	// Update reachability of next checkpoint
	if (!bIsFinish && CheckpointIndex + 1 < ActiveCheckpoints.Num())
	{
		if (AMZCheckpointActor* NextCheckpoint = ActiveCheckpoints[CheckpointIndex + 1])
		{
			NextCheckpoint->SetCheckpointState(true, false);
		}
	}

	if (bIsFinish)
	{
		CompleteActivity();
	}
	else
	{
		BroadcastProgressUpdate();
	}
}

void UMZActivityManager::OnDeliveryPickedUp(FName DeliveryID, bool bPickedUp)
{
	if (ActivityState != EMZActivityState::Active)
	{
		return;
	}

	if (CurrentActivity && CurrentActivity->ActivityType != EMZActivityType::DeliveryRun)
	{
		return;
	}

	bCargoPickedUp = true;
	UE_LOG(LogMileZero, Log, TEXT("Cargo picked up: %s"), *DeliveryID.ToString());

	// Activate dropoff points
	for (AMZDeliveryPickup* DropoffPoint : ActiveDeliveryPoints)
	{
		if (DropoffPoint && !DropoffPoint->GetIsPickup())
		{
			DropoffPoint->SetDeliveryActive(true);
		}
	}

	BroadcastProgressUpdate();
}

void UMZActivityManager::OnDeliveryDroppedOff(FName DeliveryID)
{
	if (ActivityState != EMZActivityState::Active)
	{
		return;
	}

	if (CurrentActivity && CurrentActivity->ActivityType != EMZActivityType::DeliveryRun)
	{
		return;
	}

	if (!bCargoPickedUp)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Dropoff attempted but cargo not picked up"));
		return;
	}

	UE_LOG(LogMileZero, Log, TEXT("Cargo dropped off: %s"), *DeliveryID.ToString());
	CompleteActivity();
}

void UMZActivityManager::IncrementTrafficVehiclesPassed()
{
	if (ActivityState != EMZActivityState::Active)
	{
		return;
	}

	if (CurrentActivity && CurrentActivity->ActivityType != EMZActivityType::TrafficWeave)
	{
		return;
	}

	++TrafficVehiclesPassed;
	BroadcastProgressUpdate();
}

void UMZActivityManager::UpdateAccumulatedDamage(float DamageAmount)
{
	if (ActivityState != EMZActivityState::Active)
	{
		return;
	}

	AccumulatedDamage += DamageAmount;

	// Check if exceeds limit for damage survival
	if (CurrentActivity && CurrentActivity->ActivityType == EMZActivityType::DamageSurvival)
	{
		if (CurrentActivity->MaxDamageAllowed > 0.0f && AccumulatedDamage >= CurrentActivity->MaxDamageAllowed)
		{
			FailActivity(TEXT("Exceeded maximum damage"));
			return;
		}
	}

	BroadcastProgressUpdate();
}

void UMZActivityManager::SetDamageFromState(const FMZDamageState& DamageState)
{
	float TotalDamage = DamageState.SteeringDamage + DamageState.EngineDamage +
		DamageState.CoolingDamage + DamageState.AeroDamage + DamageState.BrakeDamage;

	// Add wheel and suspension damage
	for (int32 i = 0; i < 4; ++i)
	{
		TotalDamage += DamageState.SuspensionDamage[i] + DamageState.WheelDamage[i];
	}

	UpdateAccumulatedDamage(TotalDamage);
}

float UMZActivityManager::GetBestTime(FName ActivityID) const
{
	const float* BestTimePtr = BestTimes.Find(ActivityID);
	return BestTimePtr ? *BestTimePtr : 0.0f;
}

FText UMZActivityManager::GetCurrentObjectiveText() const
{
	if (!CurrentActivity)
	{
		return FText::FromString("No activity");
	}

	FString ObjectiveText;

	switch (CurrentActivity->ActivityType)
	{
	case EMZActivityType::CheckpointSprint:
	{
		int32 Total = ActiveCheckpoints.Num();
		int32 Current = CurrentCheckpointIndex + 1;
		ObjectiveText = FString::Printf(TEXT("Checkpoint %d of %d"), Current, Total);
		break;
	}
	case EMZActivityType::DeliveryRun:
	{
		if (!bCargoPickedUp)
		{
			ObjectiveText = TEXT("Pick up cargo");
		}
		else
		{
			ObjectiveText = TEXT("Drop off cargo");
		}
		break;
	}
	case EMZActivityType::TrafficWeave:
	{
		ObjectiveText = FString::Printf(TEXT("Vehicles passed: %d"), TrafficVehiclesPassed);
		break;
	}
	case EMZActivityType::DamageSurvival:
	{
		float DamagePercent = (AccumulatedDamage / FMath::Max(CurrentActivity->MaxDamageAllowed, 1.0f)) * 100.0f;
		ObjectiveText = FString::Printf(TEXT("Damage: %.1f%%"), FMath::Clamp(DamagePercent, 0.0f, 100.0f));
		break;
	}
	default:
		ObjectiveText = TEXT("Unknown objective");
	}

	return FText::FromString(ObjectiveText);
}

FMZActivityHUDData UMZActivityManager::GetHUDData() const
{
	FMZActivityHUDData HUDData;
	HUDData.ObjectiveText = GetCurrentObjectiveText();
	HUDData.ElapsedTime = ElapsedTime;
	HUDData.TimeLimit = CurrentActivity ? CurrentActivity->TimeLimitSeconds : 0.0f;
	HUDData.CurrentCheckpoint = CurrentCheckpointIndex;
	HUDData.TotalCheckpoints = ActiveCheckpoints.Num();
	HUDData.DamagePercent = CurrentActivity && CurrentActivity->MaxDamageAllowed > 0.0f
		? (AccumulatedDamage / CurrentActivity->MaxDamageAllowed) * 100.0f
		: 0.0f;
	HUDData.ActivityType = CurrentActivity ? CurrentActivity->ActivityType : EMZActivityType::CheckpointSprint;
	HUDData.bIsActive = ActivityState == EMZActivityState::Active;

	return HUDData;
}

void UMZActivityManager::TickActivity(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	// Check for time limit exceeded
	if (CurrentActivity && CurrentActivity->TimeLimitSeconds > 0.0f)
	{
		if (ElapsedTime >= CurrentActivity->TimeLimitSeconds)
		{
			FailActivity(TEXT("Time limit exceeded"));
			return;
		}
	}

	// Check completion conditions
	if (CheckActivityCompletion())
	{
		CompleteActivity();
		return;
	}

	BroadcastProgressUpdate();
}

void UMZActivityManager::SetupCheckpointSprint()
{
	// Find all checkpoint actors in the level
	ActiveCheckpoints.Empty();

	for (TActorIterator<AMZCheckpointActor> It(GetWorld()); It; ++It)
	{
		if (AMZCheckpointActor* Checkpoint = *It)
		{
			ActiveCheckpoints.Add(Checkpoint);
			Checkpoint->OnCheckpointReached.AddDynamic(this, &UMZActivityManager::OnCheckpointReached);
		}
	}

	// Sort by index
	ActiveCheckpoints.Sort([](const AMZCheckpointActor& A, const AMZCheckpointActor& B)
	{
		return A.GetCheckpointIndex() < B.GetCheckpointIndex();
	});

	UE_LOG(LogMileZero, Log, TEXT("Checkpoint Sprint setup: %d checkpoints"), ActiveCheckpoints.Num());

	// Set first checkpoint as reachable
	if (ActiveCheckpoints.Num() > 0)
	{
		CurrentCheckpointIndex = -1;
		ActiveCheckpoints[0]->SetCheckpointState(true, false);
	}
}

void UMZActivityManager::SetupDeliveryRun()
{
	// Find all delivery points
	ActiveDeliveryPoints.Empty();

	for (TActorIterator<AMZDeliveryPickup> It(GetWorld()); It; ++It)
	{
		if (AMZDeliveryPickup* DeliveryPoint = *It)
		{
			ActiveDeliveryPoints.Add(DeliveryPoint);
			DeliveryPoint->OnDeliveryInteraction.AddDynamic(this, &UMZActivityManager::OnDeliveryPickedUp);

			// Disable dropoffs initially (enable after pickup)
			if (!DeliveryPoint->GetIsPickup())
			{
				DeliveryPoint->SetDeliveryActive(false);
			}
		}
	}

	UE_LOG(LogMileZero, Log, TEXT("Delivery Run setup: %d delivery points"), ActiveDeliveryPoints.Num());
}

void UMZActivityManager::SetupTrafficWeave()
{
	TrafficVehiclesPassed = 0;
	UE_LOG(LogMileZero, Log, TEXT("Traffic Weave setup"));
}

void UMZActivityManager::SetupDamageSurvival()
{
	AccumulatedDamage = 0.0f;
	UE_LOG(LogMileZero, Log, TEXT("Damage Survival setup"));
}

bool UMZActivityManager::CheckActivityCompletion()
{
	if (!CurrentActivity)
	{
		return false;
	}

	// For traffic weave, completion is 0 (must be managed externally via IncrementTrafficVehiclesPassed)
	// This is intentional - the activity stays active and the HUD shows progress
	return false;
}

bool UMZActivityManager::CheckActivityFailure()
{
	return false; // Explicit checks happen in Tick
}

void UMZActivityManager::ClearActivityState()
{
	// Unbind delegates from checkpoints
	for (AMZCheckpointActor* Checkpoint : ActiveCheckpoints)
	{
		if (Checkpoint)
		{
			Checkpoint->OnCheckpointReached.RemoveDynamic(this, &UMZActivityManager::OnCheckpointReached);
		}
	}
	ActiveCheckpoints.Empty();

	// Unbind delegates from delivery points
	for (AMZDeliveryPickup* DeliveryPoint : ActiveDeliveryPoints)
	{
		if (DeliveryPoint)
		{
			DeliveryPoint->OnDeliveryInteraction.RemoveDynamic(this, &UMZActivityManager::OnDeliveryPickedUp);
		}
	}
	ActiveDeliveryPoints.Empty();

	CurrentCheckpointIndex = -1;
	bCargoPickedUp = false;
	TrafficVehiclesPassed = 0;
}

void UMZActivityManager::BroadcastProgressUpdate()
{
	OnActivityProgressUpdated.Broadcast(GetHUDData());
}
