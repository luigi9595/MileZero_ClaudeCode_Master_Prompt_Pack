#include "MZDeliveryPickup.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MileZero.h"

AMZDeliveryPickup::AMZDeliveryPickup()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.0f;

	// Root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	// Pickup/dropoff zone
	PickupZone = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupZone"));
	PickupZone->SetupAttachment(RootComponent);
	PickupZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupZone->SetCollisionObjectType(ECC_WorldDynamic);
	PickupZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupZone->SetBoxExtent(FVector(250.0f, 250.0f, 200.0f));
	PickupZone->bReturnMaterialOnMove = true;
	PickupZone->OnComponentBeginOverlap.AddDynamic(this, &AMZDeliveryPickup::OnPickupZoneBeginOverlap);

	// Visual mesh
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMZDeliveryPickup::BeginPlay()
{
	Super::BeginPlay();
	SetDeliveryActive(true);
}

void AMZDeliveryPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMZDeliveryPickup::OnPickupZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	// Only care about pawns (vehicles)
	if (!OtherActor->IsA<APawn>())
	{
		return;
	}

	OnDeliveryInteraction.Broadcast(DeliveryID, bIsPickup);

	UE_LOG(LogMileZero, Log, TEXT("Delivery %s at %s"),
		bIsPickup ? TEXT("pickup") : TEXT("dropoff"), *DeliveryID.ToString());
}

void AMZDeliveryPickup::SetDeliveryActive(bool bActive)
{
	// Can enable/disable the zone based on activity state
	if (PickupZone)
	{
		PickupZone->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	// Update visual color
	if (VisualMesh)
	{
		FLinearColor TargetColor = bIsPickup ? PickupColor : DropoffColor;
		if (!bActive)
		{
			TargetColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.5f); // Dim when inactive
		}

		UMaterialInstanceDynamic* DynMaterial = VisualMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMaterial)
		{
			DynMaterial->SetVectorParameterValue(FName("BaseColor"), TargetColor);
		}
	}
}
