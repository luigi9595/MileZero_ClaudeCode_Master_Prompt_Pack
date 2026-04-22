#include "MZNotificationManager.h"
#include "MileZero.h"

void UMZNotificationManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActiveNotifications.Reserve(MAX_NOTIFICATIONS);
	UE_LOG(LogMileZero, Log, TEXT("MZNotificationManager initialized"));
}

void UMZNotificationManager::Deinitialize()
{
	ActiveNotifications.Empty();
	Super::Deinitialize();
}

void UMZNotificationManager::ShowNotification(const FText& Message, EMZNotificationType Type, float Duration)
{
	if (ActiveNotifications.Num() >= MAX_NOTIFICATIONS)
	{
		// Remove oldest notification
		ActiveNotifications.RemoveAt(0);
		UE_LOG(LogMileZero, Warning, TEXT("Notification queue full, removing oldest"));
	}

	FMZNotification NewNotif;
	NewNotif.Message = Message;
	NewNotif.Type = Type;
	NewNotif.Duration = Duration;
	NewNotif.ElapsedTime = 0.0f;
	NewNotif.Color = GetColorForType(Type);

	ActiveNotifications.Add(NewNotif);
	UE_LOG(LogMileZero, Log, TEXT("Notification shown: %s"), *Message.ToString());
}

void UMZNotificationManager::TickNotifications(float DeltaTime)
{
	for (int32 i = ActiveNotifications.Num() - 1; i >= 0; --i)
	{
		FMZNotification& Notif = ActiveNotifications[i];
		Notif.ElapsedTime += DeltaTime;

		if (Notif.ElapsedTime >= Notif.Duration)
		{
			ActiveNotifications.RemoveAt(i);
		}
	}
}

void UMZNotificationManager::NotifyZoneEntered(const FText& ZoneName)
{
	FText Message = FText::FromString(FString::Printf(TEXT("Entered: %s"), *ZoneName.ToString()));
	ShowNotification(Message, EMZNotificationType::Zone, 2.5f);
}

void UMZNotificationManager::NotifyMoney(float Amount, const FString& Reason)
{
	EMZNotificationType Type = (Amount >= 0.0f) ? EMZNotificationType::Success : EMZNotificationType::Warning;
	FString Sign = (Amount >= 0.0f) ? TEXT("+") : TEXT("");
	FString ReasonStr = Reason.IsEmpty() ? TEXT("") : FString::Printf(TEXT(" - %s"), *Reason);
	FText Message = FText::FromString(FString::Printf(TEXT("%s$%.2f%s"), *Sign, FMath::Abs(Amount), *ReasonStr));
	ShowNotification(Message, EMZNotificationType::Money, 2.0f);
}

void UMZNotificationManager::NotifyActivityStart(const FText& ActivityName)
{
	FText Message = FText::FromString(FString::Printf(TEXT("Activity Started: %s"), *ActivityName.ToString()));
	ShowNotification(Message, EMZNotificationType::Activity, 3.0f);
}

void UMZNotificationManager::NotifyActivityComplete(const FText& ActivityName, float Time)
{
	FText Message = FText::FromString(FString::Printf(TEXT("Activity Complete: %s (%.2f s)"), *ActivityName.ToString(), Time));
	ShowNotification(Message, EMZNotificationType::Success, 3.5f);
}

FLinearColor UMZNotificationManager::GetColorForType(EMZNotificationType Type)
{
	switch (Type)
	{
		case EMZNotificationType::Info:
			return FLinearColor::White;
		case EMZNotificationType::Success:
			return FLinearColor::Green;
		case EMZNotificationType::Warning:
			return FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // Orange
		case EMZNotificationType::Money:
			return FLinearColor(1.0f, 0.84f, 0.0f, 1.0f); // Gold
		case EMZNotificationType::Activity:
			return FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
		case EMZNotificationType::Zone:
			return FLinearColor(0.63f, 0.0f, 1.0f, 1.0f); // Purple
		default:
			return FLinearColor::White;
	}
}
