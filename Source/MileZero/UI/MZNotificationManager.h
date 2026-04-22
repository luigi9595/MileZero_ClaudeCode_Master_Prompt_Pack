#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZNotificationManager.generated.h"

UENUM(BlueprintType)
enum class EMZNotificationType : uint8
{
	Info = 0,
	Success = 1,
	Warning = 2,
	Money = 3,
	Activity = 4,
	Zone = 5
};

USTRUCT(BlueprintType)
struct FMZNotification
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText Message;

	UPROPERTY(BlueprintReadOnly)
	float Duration = 3.0f;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly)
	float ElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	EMZNotificationType Type = EMZNotificationType::Info;
};

/**
 * UMZNotificationManager: Manages on-screen notifications.
 * Persistent GameInstanceSubsystem that tracks active notifications and their lifetime.
 */
UCLASS()
class MILEZERO_API UMZNotificationManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Show a new notification on screen.
	 * @param Message The text to display
	 * @param Type The notification type (determines color/icon)
	 * @param Duration How long to show (seconds)
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Notifications")
	void ShowNotification(const FText& Message, EMZNotificationType Type = EMZNotificationType::Info, float Duration = 3.0f);

	/**
	 * Tick notifications each frame (removes expired ones).
	 * Called from the notification widget's Tick.
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Notifications")
	void TickNotifications(float DeltaTime);

	/**
	 * Get the array of active notifications.
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Notifications")
	const TArray<FMZNotification>& GetActiveNotifications() const { return ActiveNotifications; }

	/**
	 * Predefined helper: notify zone entrance
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Notifications")
	void NotifyZoneEntered(const FText& ZoneName);

	/**
	 * Predefined helper: notify money earn/spend
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Notifications")
	void NotifyMoney(float Amount, const FString& Reason = TEXT(""));

	/**
	 * Predefined helper: notify activity started
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Notifications")
	void NotifyActivityStart(const FText& ActivityName);

	/**
	 * Predefined helper: notify activity completed with time
	 */
	UFUNCTION(BlueprintCallable, Category = "MZ|Notifications")
	void NotifyActivityComplete(const FText& ActivityName, float Time);

	/**
	 * Get color for a notification type
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Notifications")
	static FLinearColor GetColorForType(EMZNotificationType Type);

private:
	// Max 5 visible notifications
	static constexpr int32 MAX_NOTIFICATIONS = 5;

	UPROPERTY()
	TArray<FMZNotification> ActiveNotifications;
};
