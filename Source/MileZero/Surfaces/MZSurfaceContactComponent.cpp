#include "MZSurfaceContactComponent.h"
#include "MZSurfaceDataAsset.h"
#include "MileZero.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/World.h"

UMZSurfaceContactComponent::UMZSurfaceContactComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f; // 20 Hz is sufficient for surface detection
}

void UMZSurfaceContactComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentSurface = DefaultSurface;
}

void UMZSurfaceContactComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DetectSurface();
}

void UMZSurfaceContactComponent::DetectSurface()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Trace downward from vehicle center to detect ground material
	FVector Start = Owner->GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 200.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	Params.bReturnPhysicalMaterial = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		if (Hit.PhysMaterial.IsValid())
		{
			FName MatName = Hit.PhysMaterial->GetFName();
			if (const TObjectPtr<UMZSurfaceDataAsset>* Found = SurfaceLookup.Find(MatName))
			{
				CurrentSurface = *Found;
				return;
			}
		}
	}

	// Fallback to default surface
	CurrentSurface = DefaultSurface;
}

float UMZSurfaceContactComponent::GetCurrentGripMultiplier() const
{
	return CurrentSurface ? CurrentSurface->GripMultiplier : 1.0f;
}

FName UMZSurfaceContactComponent::GetCurrentSurfaceID() const
{
	return CurrentSurface ? CurrentSurface->SurfaceID : FName("Unknown");
}
