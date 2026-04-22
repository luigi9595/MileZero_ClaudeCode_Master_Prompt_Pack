#include "MZEconomySubsystem.h"
#include "MileZero.h"

DEFINE_LOG_CATEGORY_STATIC(LogMZEconomy, Log, All);

void UMZEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Start with default money
	CurrentMoney = STARTING_MONEY;
	Transactions.Empty();

	UE_LOG(LogMileZero, Log, TEXT("UMZEconomySubsystem initialized with $%.2f"), CurrentMoney);
}

void UMZEconomySubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogMileZero, Log, TEXT("UMZEconomySubsystem deinitialized"));
}

void UMZEconomySubsystem::AddMoney(float Amount, const FString& Reason)
{
	if (Amount <= 0.0f)
	{
		UE_LOG(LogMileZero, Warning, TEXT("AddMoney called with non-positive amount: %.2f"), Amount);
		return;
	}

	const float PreviousMoney = CurrentMoney;
	CurrentMoney += Amount;

	RecordTransaction(Amount, Reason, false);

	UE_LOG(LogMileZero, Log, TEXT("AddMoney: +$%.2f (%s) | Balance: $%.2f -> $%.2f"),
		Amount, *Reason, PreviousMoney, CurrentMoney);

	OnMoneyChanged.Broadcast(CurrentMoney, Amount);
}

bool UMZEconomySubsystem::SpendMoney(float Amount, const FString& Reason)
{
	if (Amount <= 0.0f)
	{
		UE_LOG(LogMileZero, Warning, TEXT("SpendMoney called with non-positive amount: %.2f"), Amount);
		return false;
	}

	if (CurrentMoney < Amount)
	{
		UE_LOG(LogMileZero, Warning, TEXT("SpendMoney failed: insufficient funds. Have $%.2f, need $%.2f (%s)"),
			CurrentMoney, Amount, *Reason);
		return false;
	}

	const float PreviousMoney = CurrentMoney;
	CurrentMoney -= Amount;

	RecordTransaction(Amount, Reason, true);

	UE_LOG(LogMileZero, Log, TEXT("SpendMoney: -$%.2f (%s) | Balance: $%.2f -> $%.2f"),
		Amount, *Reason, PreviousMoney, CurrentMoney);

	OnMoneyChanged.Broadcast(CurrentMoney, -Amount);
	return true;
}

bool UMZEconomySubsystem::CanAfford(float Amount) const
{
	return CurrentMoney >= Amount;
}

void UMZEconomySubsystem::RewardCheckpointSprint(bool bBeatBestTime)
{
	float Reward = REWARD_CHECKPOINT_SPRINT;
	if (bBeatBestTime)
	{
		Reward *= BEST_TIME_MULTIPLIER;
	}
	AddMoney(Reward, bBeatBestTime ? TEXT("Checkpoint Sprint (Best Time!)") : TEXT("Checkpoint Sprint"));
}

void UMZEconomySubsystem::RewardDeliveryRun(bool bBeatBestTime)
{
	float Reward = REWARD_DELIVERY_RUN;
	if (bBeatBestTime)
	{
		Reward *= BEST_TIME_MULTIPLIER;
	}
	AddMoney(Reward, bBeatBestTime ? TEXT("Delivery Run (Best Time!)") : TEXT("Delivery Run"));
}

void UMZEconomySubsystem::RewardTrafficWeaving(bool bBeatBestTime)
{
	float Reward = REWARD_TRAFFIC_WEAVING;
	if (bBeatBestTime)
	{
		Reward *= BEST_TIME_MULTIPLIER;
	}
	AddMoney(Reward, bBeatBestTime ? TEXT("Traffic Weaving (Best Time!)") : TEXT("Traffic Weaving"));
}

void UMZEconomySubsystem::RewardDamageSurvival(bool bBeatBestTime)
{
	float Reward = REWARD_DAMAGE_SURVIVAL;
	if (bBeatBestTime)
	{
		Reward *= BEST_TIME_MULTIPLIER;
	}
	AddMoney(Reward, bBeatBestTime ? TEXT("Damage Survival (Best Time!)") : TEXT("Damage Survival"));
}

void UMZEconomySubsystem::ApplyTrafficCollisionFine()
{
	SpendMoney(FINE_TRAFFIC_COLLISION, TEXT("Traffic Collision Fine"));
}

TArray<FMZTransaction> UMZEconomySubsystem::GetRecentTransactions() const
{
	return Transactions;
}

void UMZEconomySubsystem::GetSessionEconomics(float& OutTotalEarned, float& OutTotalSpent) const
{
	OutTotalEarned = 0.0f;
	OutTotalSpent = 0.0f;

	for (const FMZTransaction& Trans : Transactions)
	{
		if (Trans.bIsExpense)
		{
			OutTotalSpent += Trans.Amount;
		}
		else
		{
			OutTotalEarned += Trans.Amount;
		}
	}
}

void UMZEconomySubsystem::ResetEconomy()
{
	CurrentMoney = STARTING_MONEY;
	Transactions.Empty();
	OnMoneyChanged.Broadcast(CurrentMoney, 0.0f);
	UE_LOG(LogMileZero, Log, TEXT("Economy reset to $%.2f"), CurrentMoney);
}

void UMZEconomySubsystem::RecordTransaction(float Amount, const FString& Reason, bool bIsExpense)
{
	FMZTransaction NewTransaction;
	NewTransaction.Amount = Amount;
	NewTransaction.Reason = Reason;
	NewTransaction.Timestamp = FDateTime::Now();
	NewTransaction.bIsExpense = bIsExpense;

	Transactions.Add(NewTransaction);

	// Keep only recent history
	if (Transactions.Num() > MAX_TRANSACTION_HISTORY)
	{
		Transactions.RemoveAt(0);
	}
}
