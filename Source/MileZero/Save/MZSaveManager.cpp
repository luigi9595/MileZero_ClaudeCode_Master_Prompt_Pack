#include "MZSaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "MileZero.h"

void UMZSaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Try to load existing save
	if (HasSaveData())
	{
		LoadGame();
	}
	else
	{
		// Create new save
		CurrentSaveGame = NewObject<UMZSaveGame>();
	}

	PlaytimeAccumulator = 0.0f;

	UE_LOG(LogMileZero, Log, TEXT("Save Manager initialized"));
}

void UMZSaveManager::Deinitialize()
{
	// Auto-save on shutdown
	AutoSave();
	Super::Deinitialize();
}

bool UMZSaveManager::SaveGame(const FString& SlotName)
{
	if (!CurrentSaveGame)
	{
		CurrentSaveGame = NewObject<UMZSaveGame>();
	}

	FString ActiveSlot = GetActiveSlotName(SlotName);

	// Capture current state
	CaptureGameState(CurrentSaveGame);

	// Save to disk
	bool bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, ActiveSlot, 0);

	if (bSuccess)
	{
		UE_LOG(LogMileZero, Log, TEXT("Game saved to slot: %s"), *ActiveSlot);
		OnSaveStateChanged.Broadcast(true, FString::Printf(TEXT("Game saved to %s"), *ActiveSlot));
	}
	else
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to save game to slot: %s"), *ActiveSlot);
		OnSaveStateChanged.Broadcast(false, FString::Printf(TEXT("Failed to save to %s"), *ActiveSlot));
	}

	return bSuccess;
}

bool UMZSaveManager::LoadGame(const FString& SlotName)
{
	FString ActiveSlot = GetActiveSlotName(SlotName);

	// Load from disk
	CurrentSaveGame = Cast<UMZSaveGame>(
		UGameplayStatics::LoadGameFromSlot(ActiveSlot, 0)
	);

	if (!CurrentSaveGame)
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to load game from slot: %s"), *ActiveSlot);
		OnSaveStateChanged.Broadcast(false, FString::Printf(TEXT("Failed to load from %s"), *ActiveSlot));
		return false;
	}

	// Apply state to world
	ApplyGameState(CurrentSaveGame);

	UE_LOG(LogMileZero, Log, TEXT("Game loaded from slot: %s"), *ActiveSlot);
	OnSaveStateChanged.Broadcast(true, FString::Printf(TEXT("Game loaded from %s"), *ActiveSlot));

	PlaytimeAccumulator = 0.0f;
	return true;
}

bool UMZSaveManager::AutoSave()
{
	return SaveGame();
}

bool UMZSaveManager::HasSaveData(const FString& SlotName) const
{
	FString ActiveSlot = GetActiveSlotName(SlotName);
	return UGameplayStatics::DoesSaveGameExist(ActiveSlot, 0);
}

bool UMZSaveManager::DeleteSave(const FString& SlotName)
{
	FString ActiveSlot = GetActiveSlotName(SlotName);
	bool bSuccess = UGameplayStatics::DeleteGameInSlot(ActiveSlot, 0);

	if (bSuccess)
	{
		UE_LOG(LogMileZero, Log, TEXT("Save slot deleted: %s"), *ActiveSlot);
		OnSaveStateChanged.Broadcast(true, FString::Printf(TEXT("Save slot %s deleted"), *ActiveSlot));
	}
	else
	{
		UE_LOG(LogMileZero, Warning, TEXT("Failed to delete save slot: %s"), *ActiveSlot);
		OnSaveStateChanged.Broadcast(false, FString::Printf(TEXT("Failed to delete %s"), *ActiveSlot));
	}

	return bSuccess;
}

void UMZSaveManager::UpdatePlaytime(float DeltaTime)
{
	if (!CurrentSaveGame)
	{
		return;
	}

	PlaytimeAccumulator += DeltaTime;
	CurrentSaveGame->TotalPlayTimeSeconds += DeltaTime;

	// Autosave every interval
	if (PlaytimeAccumulator >= AutosaveIntervalSeconds)
	{
		PlaytimeAccumulator = 0.0f;
		AutoSave();
	}
}

void UMZSaveManager::CaptureGameState(UMZSaveGame* SaveGame)
{
	if (!SaveGame)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Capture world name
	SaveGame->CurrentMapName = World->GetMapName();

	// Capture player pawn transform
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		SaveGame->PlayerTransform = PC->GetPawn()->GetActorTransform();
	}

	// LastVehicleID and other data should be managed by the game mode/vehicle system
	// This is a placeholder - the actual capture would be done by the vehicle manager

	UE_LOG(LogMileZero, Log, TEXT("Game state captured"));
}

void UMZSaveManager::ApplyGameState(const UMZSaveGame* SaveGame)
{
	if (!SaveGame)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Apply player transform if we're in the saved map
	APlayerController* PC = World->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		PC->GetPawn()->SetActorTransform(SaveGame->PlayerTransform);
	}

	UE_LOG(LogMileZero, Log, TEXT("Game state applied"));
}

FString UMZSaveManager::GetActiveSlotName(const FString& SlotName) const
{
	if (SlotName.IsEmpty())
	{
		return UMZSaveGame::DefaultSlotName;
	}
	return SlotName;
}
