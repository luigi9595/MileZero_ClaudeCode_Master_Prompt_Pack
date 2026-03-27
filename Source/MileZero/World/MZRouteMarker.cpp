#include "MZRouteMarker.h"
#include "MileZero.h"
#include "Components/BillboardComponent.h"

AMZRouteMarker::AMZRouteMarker()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	// Create and set up the billboard component for editor visibility
	EditorIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIcon"));
	RootComponent = EditorIcon;
	EditorIcon->SetIsVisualizationComponent(true);
	EditorIcon->SetAbsolute(false, false, true);

	// Set up default billboard text
	SetActorLabel(TEXT("RouteMarker"));

	// Make actor non-physical
	SetActorEnableCollision(false);
}

void AMZRouteMarker::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogMileZero, Display, TEXT("RouteMarker [%s] initialized | Seq: %d | %s -> %s | Location: (%.0f, %.0f, %.0f)"),
	       *RouteID.ToString(), SequenceIndex, *FromZoneID.ToString(), *ToZoneID.ToString(),
	       GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
}

#if WITH_EDITOR
void AMZRouteMarker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Update billboard text to reflect route info
	if (EditorIcon)
	{
		FString BillboardText = FString::Printf(TEXT("Route [%s] #%d"), *RouteID.ToString(), SequenceIndex);
		SetActorLabel(*BillboardText);
	}
}
#endif

FString AMZRouteMarker::GetRouteMarkerInfo() const
{
	return FString::Printf(TEXT("RouteMarker: %s | Seq: %d | %s -> %s | Location: (%.0f, %.0f, %.0f)"),
	                        *RouteID.ToString(), SequenceIndex, *FromZoneID.ToString(), *ToZoneID.ToString(),
	                        GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
}

float AMZRouteMarker::GetDistanceToMarker(const AMZRouteMarker* OtherMarker) const
{
	if (!OtherMarker)
	{
		return 0.0f;
	}

	return FVector::Distance(GetActorLocation(), OtherMarker->GetActorLocation());
}
