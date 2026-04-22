#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZEconomyTypes.h"
#include "MZEconomySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoneyChanged, float, NewAmount, float, Delta);

/**
 * Manages player economy: money, transactions, activity rewards, and fines.
 * Persisted across map changes via GameInstanceSubsystem.
 */
UCLASS()
class MILEZERO_API UMZEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Add money to the player account
	 * @param Amount - positive amount to add
	 * @param Reason - log description (e.g., "Checkpoint Sprint Reward")
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AddMoney(float Amount, const FString& Reason);

	/**
	 * Spend money if affordable
	 * @param Amount - positive amount to spend
	 * @param Reason - log description (e.g., "Vehicle Repair")
	 * @return true if spend succeeded, false if insufficient funds
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool SpendMoney(float Amount, const FString& Reason);

	/**
	 * Check if player can afford an amount
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool CanAfford(float Amount) const;

	/**
	 * Get current money
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	float GetCurrentMoney() const { return CurrentMoney; }

	/**
	 * Activity reward methods - called by activity manager
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy|Activities")
	void RewardCheckpointSprint(bool bBeatBestTime = false);

	UFUNCTION(BlueprintCallable, Category = "Economy|Activities")
	void RewardDeliveryRun(bool bBeatBestTime = false);

	UFUNCTION(BlueprintCallable, Category = "Economy|Activities")
	void RewardTrafficWeaving(bool bBeatBestTime = false);

	UFUNCTION(BlueprintCallable, Category = "Economy|Activities")
	void RewardDamageSurvival(bool bBeatBestTime = false);

	/**
	 * Apply fine for traffic collisions
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy|Fines")
	void ApplyTrafficCollisionFine();

	/**
	 * Get recent transaction history (last 50)
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	TArray<FMZTransaction> GetRecentTransactions() const;

	/**
	 * Get summary of income/expenses for current session
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void GetSessionEconomics(float& OutTotalEarned, float& OutTotalSpent) const;

	/**
	 * Reset economy to starting state (testing/new game)
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void ResetEconomy();

	// Delegate fired when money changes
	UPROPERTY(BlueprintAssignable, Category = "Economy")
	FOnMoneyChanged OnMoneyChanged;

protected:
	// Current money in player account
	UPROPERTY(BlueprintReadOnly, Category = "Economy")
	float CurrentMoney = 5000.0f;

	// Transaction history
	UPROPERTY(BlueprintReadOnly, Category = "Economy")
	TArray<FMZTransaction> Transactions;

	// Starting money for reset
	static constexpr float STARTING_MONEY = 5000.0f;

	// Activity reward amounts (base)
	static constexpr float REWARD_CHECKPOINT_SPRINT = 500.0f;
	static constexpr float REWARD_DELIVERY_RUN = 750.0f;
	static constexpr float REWARD_TRAFFIC_WEAVING = 600.0f;
	static constexpr float REWARD_DAMAGE_SURVIVAL = 1000.0f;

	// Best time bonus multiplier
	static constexpr float BEST_TIME_MULTIPLIER = 2.0f;

	// Traffic collision fine
	static constexpr float FINE_TRAFFIC_COLLISION = 50.0f;

	// Max transactions to keep in history
	static constexpr int32 MAX_TRANSACTION_HISTORY = 50;

	/**
	 * Internal method to record a transaction
	 */
	void RecordTransaction(float Amount, const FString& Reason, bool bIsExpense);
};
