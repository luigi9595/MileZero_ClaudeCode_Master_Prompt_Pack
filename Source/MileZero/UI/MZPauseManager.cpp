#include "MZPauseManager.h"
#include "MZPauseMenuWidget.h"
#include "MileZero.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/GameplayStatics.h"

void UMZPauseManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bIsPaused = false;
	UE_LOG(LogMileZero, Log, TEXT("MZPauseManager initialized"));
}

void UMZPauseManager::Deinitialize()
{
	HidePauseMenu();
	Super::Deinitialize();
}

void UMZPauseManager::TogglePause()
{
	if (bIsPaused)
	{
		Resume();
	}
	else
	{
		bIsPaused = true;
		ShowPauseMenu();

		// Set game to paused
		if (UWorld* World = GetWorld())
		{
			World->GetFirstPlayerController()->SetPause(true);
		}

		UE_LOG(LogMileZero, Log, TEXT("Game paused"));
	}
}

void UMZPauseManager::Resume()
{
	if (!bIsPaused) return;

	bIsPaused = false;
	HidePauseMenu();

	// Resume game
	if (UWorld* World = GetWorld())
	{
		World->GetFirstPlayerController()->SetPause(false);
	}

	UE_LOG(LogMileZero, Log, TEXT("Game resumed"));
}

void UMZPauseManager::QuitToDesktop()
{
	UE_LOG(LogMileZero, Log, TEXT("Quit to desktop requested"));

	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, FName(""), false);
		World->GetFirstPlayerController()->ConsoleCommand(TEXT("quit"));
	}
}

void UMZPauseManager::ShowPauseMenu()
{
	if (GEngine && GEngine->GameViewport)
	{
		PauseMenuWidget = SNew(SMZPauseMenuWidget)
			.PauseManager(this);

		GEngine->GameViewport->AddViewportWidgetContent(PauseMenuWidget.ToSharedRef());

		// Show mouse cursor
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
		}

		UE_LOG(LogMileZero, Log, TEXT("Pause menu shown"));
	}
}

void UMZPauseManager::HidePauseMenu()
{
	if (PauseMenuWidget.IsValid() && GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(PauseMenuWidget.ToSharedRef());
		PauseMenuWidget.Reset();

		// Hide mouse cursor
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}

		UE_LOG(LogMileZero, Log, TEXT("Pause menu hidden"));
	}
}
