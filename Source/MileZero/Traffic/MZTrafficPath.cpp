#include "MZTrafficPath.h"
#include "MileZero.h"
#include "Components/SplineComponent.h"

AMZTrafficPath::AMZTrafficPath()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the spline component
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathSpline->SetupAttachment(RootComponent);
	RootComponent = PathSpline;

	// Default settings
	PathSpline->SetClosedLoop(false);
	PathSpline->bDrawDebug = true;
	PathSpline->SetUnselectedSplineSegmentColor(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow
}

void AMZTrafficPath::BeginPlay()
{
	Super::BeginPlay();

	if (!IsPathValid())
	{
		UE_LOG(LogMileZero, Warning, TEXT("MZTrafficPath %s is invalid or has insufficient points"), *GetName());
	}
}

void AMZTrafficPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AMZTrafficPath::GetLocationAtDistance(float Distance) const
{
	if (!PathSpline || PathSpline->GetNumberOfSplinePoints() < 2)
	{
		return GetActorLocation();
	}

	float PathLength = PathSpline->GetSplineLength();
	float ClampedDistance = FMath::Clamp(Distance, 0.0f, PathLength);
	return PathSpline->GetLocationAtDistanceAlongSpline(ClampedDistance, ESplineCoordinateSpace::World);
}

FVector AMZTrafficPath::GetDirectionAtDistance(float Distance) const
{
	if (!PathSpline || PathSpline->GetNumberOfSplinePoints() < 2)
	{
		return GetActorForwardVector();
	}

	float PathLength = PathSpline->GetSplineLength();
	float ClampedDistance = FMath::Clamp(Distance, 0.0f, PathLength);
	return PathSpline->GetTangentAtDistanceAlongSpline(ClampedDistance, ESplineCoordinateSpace::World).GetSafeNormal();
}

float AMZTrafficPath::GetClosestDistanceOnPath(FVector WorldLocation) const
{
	if (!PathSpline || PathSpline->GetNumberOfSplinePoints() < 2)
	{
		return 0.0f;
	}

	return PathSpline->FindInputKeyClosestToWorldLocation(WorldLocation);
}

FVector AMZTrafficPath::GetClosestLocationOnPath(FVector WorldLocation) const
{
	if (!PathSpline || PathSpline->GetNumberOfSplinePoints() < 2)
	{
		return GetActorLocation();
	}

	float ClosestDistance = GetClosestDistanceOnPath(WorldLocation);
	return GetLocationAtDistance(ClosestDistance);
}

float AMZTrafficPath::GetPathLength() const
{
	if (!PathSpline)
	{
		return 0.0f;
	}

	return PathSpline->GetSplineLength();
}

FVector AMZTrafficPath::GetRandomStartLocation() const
{
	if (!PathSpline)
	{
		return GetActorLocation();
	}

	float PathLength = PathSpline->GetSplineLength();
	if (PathLength <= 0.0f)
	{
		return GetActorLocation();
	}

	float RandomDistance = FMath::RandRange(0.0f, PathLength * 0.3f); // Spawn in first 30% of path
	return GetLocationAtDistance(RandomDistance);
}

bool AMZTrafficPath::IsPathValid() const
{
	if (!PathSpline)
	{
		return false;
	}

	if (PathSpline->GetNumberOfSplinePoints() < 2)
	{
		return false;
	}

	return PathSpline->GetSplineLength() > 0.0f;
}
