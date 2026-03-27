#include "MZSpawnPoint.h"
#include "MileZero.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"

AMZSpawnPoint::AMZSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	// Create and set up the arrow component to show direction
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	RootComponent = DirectionArrow;
	DirectionArrow->ArrowLength = 100.0f;
	DirectionArrow->ArrowSize = 40.0f;
	DirectionArrow->SetIsVisualizationComponent(true);
	DirectionArrow->SetAbsolute(false, false, true);

	// Create and set up the billboard component for visibility in editor
	EditorSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));
	EditorSprite->SetupAttachment(RootComponent);
	EditorSprite->SetIsVisualizationComponent(true);
	EditorSprite->SetAbsolute(false, false, true);

	// Set up default billboard text
	SetActorLabel(TEXT("SpawnPoint"));

	// Make actor non-physical in the world
	SetActorEnableCollision(false);
}

void AMZSpawnPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Sync spawn location from actor transform
	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation();

	UE_LOG(LogMileZero, Display, TEXT("SpawnPoint initialized at (%.0f, %.0f, %.0f) in zone [%s] | Default: %s | Recovery: %s"),
	       SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z,
	       *AssociatedZoneID.ToString(),
	       bIsDefaultSpawn ? TEXT("Yes") : TEXT("No"),
	       bIsRecoveryPoint ? TEXT("Yes") : TEXT("No"));
}

#if WITH_EDITOR
void AMZSpawnPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Sync spawn location when actor is moved in editor
	if (PropertyChangedEvent.Property == nullptr ||
	    PropertyChangedEvent.Property->GetFName() == FName(TEXT("RelativeLocation")) ||
	    PropertyChangedEvent.Property->GetFName() == FName(TEXT("RelativeRotation")))
	{
		SpawnLocation = GetActorLocation();
		SpawnRotation = GetActorRotation();
	}

	// Update billboard text to reflect state
	if (EditorSprite)
	{
		FString BillboardText = TEXT("Spawn");
		if (bIsDefaultSpawn)
		{
			BillboardText += TEXT(" (DEFAULT)");
		}
		if (bIsRecoveryPoint)
		{
			BillboardText += TEXT(" [Recovery]");
		}
		// Billboard text label (editor-only, not supported on UBillboardComponent in UE 5.7)
		SetActorLabel(*BillboardText);
	}
}

void AMZSpawnPoint::EditorApplyRotation(const FRotator& DeltaRotation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	Super::EditorApplyRotation(DeltaRotation, bAltDown, bShiftDown, bCtrlDown);

	// Keep spawn rotation in sync
	SpawnRotation = GetActorRotation();
}
#endif

FTransform AMZSpawnPoint::GetSpawnTransform() const
{
	return FTransform(SpawnRotation, SpawnLocation, FVector::OneVector);
}

FString AMZSpawnPoint::GetSpawnPointInfo() const
{
	return FString::Printf(TEXT("SpawnPoint: (%.0f, %.0f, %.0f) | Rotation: (%.0f, %.0f, %.0f) | Zone: %s | Default: %s | Recovery: %s"),
	                        SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z,
	                        SpawnRotation.Pitch, SpawnRotation.Yaw, SpawnRotation.Roll,
	                        *AssociatedZoneID.ToString(),
	                        bIsDefaultSpawn ? TEXT("Yes") : TEXT("No"),
	                        bIsRecoveryPoint ? TEXT("Yes") : TEXT("No"));
}
