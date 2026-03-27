#include "MZCheckpointActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MileZero.h"

AMZCheckpointActor::AMZCheckpointActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.0f; // Tick every frame for visual updates if needed

	// Root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	// Trigger volume
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerVolume->bReturnMaterialOnMove = true;
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AMZCheckpointActor::OnTriggerBeginOverlap);

	// Visual mesh (optional)
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMZCheckpointActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialize visual state: start checkpoints are reachable by default
	if (bIsStart)
	{
		bIsReachable = true;
		SetCheckpointState(true, false);
	}
	else
	{
		SetCheckpointState(false, false);
	}
}

void AMZCheckpointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Could add pulsing or other visual feedback here in the future
}

void AMZCheckpointActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	// Only care about pawns (vehicle or character)
	if (!OtherActor->IsA<APawn>())
	{
		return;
	}

	// Only fire if reachable
	if (!bIsReachable)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Checkpoint %d reached but not reachable!"), CheckpointIndex);
		return;
	}

	bIsReached = true;
	OnCheckpointReached.Broadcast(CheckpointIndex, bIsFinish);

	UE_LOG(LogMileZero, Log, TEXT("Checkpoint %d reached (Finish: %s)"), CheckpointIndex, bIsFinish ? TEXT("true") : TEXT("false"));
}

void AMZCheckpointActor::SetCheckpointState(bool bReachable, bool bReached)
{
	bIsReachable = bReachable;
	bIsReached = bReached;

	// Update visual color based on state
	if (VisualMesh)
	{
		FLinearColor TargetColor = ColorUnreachable;
		if (bReached)
		{
			TargetColor = ColorPassed;
		}
		else if (bReachable)
		{
			TargetColor = ColorActive;
		}

		// Create and apply a dynamic material for the mesh
		UMaterialInstanceDynamic* DynMaterial = VisualMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMaterial)
		{
			DynMaterial->SetVectorParameterValue(FName("BaseColor"), TargetColor);
		}
	}
}
