#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MZSpawnPoint.generated.h"

class UArrowComponent;
class UBillboardComponent;

/**
 * AMZSpawnPoint: Defines a vehicle spawn/recovery location in the world.
 *
 * Spawn points are:
 * - Placed by level designers within zones
 * - Used for vehicle respawn after crashes
 * - Used for default player spawn
 * - Queried by MZWorldManager for zone-specific spawning
 *
 * Each spawn point stores its location, rotation, and zone association.
 * Visual helpers (arrow and billboard) are editor-only.
 */
UCLASS()
class MILEZERO_API AMZSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AMZSpawnPoint();

	virtual void PostInitializeComponents() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void EditorApplyRotation(const FRotator& DeltaRotation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
#endif

	// --- Spawn Configuration ---

	/** Position where vehicle will spawn (auto-synced from actor location) */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|Spawn")
	FVector SpawnLocation = FVector::ZeroVector;

	/** Rotation vehicle will have when spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Spawn")
	FRotator SpawnRotation = FRotator::ZeroRotator;

	/** Zone this spawn point belongs to (used for zone-specific spawning) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Spawn")
	FName AssociatedZoneID = NAME_None;

	/** If true, this is the default spawn point for the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Spawn")
	bool bIsDefaultSpawn = false;

	/** If true, this point is used for vehicle recovery after crash */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MZ|Spawn")
	bool bIsRecoveryPoint = true;

	// --- Editor Helpers (editor-only, not replicated) ---

	/** Arrow component showing spawn direction (editor-only) */
	UPROPERTY(VisibleAnywhere, Category = "MZ|Spawn")
	TObjectPtr<UArrowComponent> DirectionArrow;

	/** Billboard sprite visible in editor (editor-only) */
	UPROPERTY(VisibleAnywhere, Category = "MZ|Spawn")
	TObjectPtr<UBillboardComponent> EditorSprite;

	// --- API ---

	/**
	 * Get the spawn transform (location + rotation).
	 * @return Complete transform for spawning a vehicle
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Spawn")
	FTransform GetSpawnTransform() const;

	/**
	 * Get spawn info as a formatted string (for debug/telemetry).
	 * @return Info string with location, rotation, zone, and flags
	 */
	UFUNCTION(BlueprintPure, Category = "MZ|Spawn")
	FString GetSpawnPointInfo() const;
};
