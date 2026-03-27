#include "MZTrafficVehicle.h"
#include "MileZero.h"
#include "MZTrafficPath.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Controller.h"

AMZTrafficVehicle::AMZTrafficVehicle()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.05f;

	// Disable pawn defaults
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create collision box (root)
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionObjectType(ECC_Pawn);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	RootComponent = CollisionBox;

	// Create mesh component
	VehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMesh"));
	VehicleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VehicleMesh->SetupAttachment(CollisionBox);

	// Create movement component
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = CollisionBox;
	MovementComponent->MaxSpeed = 10000.0f; // Very high, will be clamped by DesiredSpeed
	MovementComponent->Acceleration = 0.0f;

	// Bind collision events
	CollisionBox->OnComponentHit.AddDynamic(this, &AMZTrafficVehicle::OnHit);
}

void AMZTrafficVehicle::BeginPlay()
{
	Super::BeginPlay();

	if (!CurrentPath)
	{
		UE_LOG(LogMileZero, Warning, TEXT("MZTrafficVehicle %s has no path assigned"), *GetName());
		return;
	}

	// Position at random start on path
	FVector StartLocation = CurrentPath->GetRandomStartLocation();
	SetActorLocation(StartLocation);

	// Face along path direction
	FVector PathDirection = CurrentPath->GetDirectionAtDistance(0.0f);
	SetActorRotation(PathDirection.Rotation());

	DistanceAlongPath = CurrentPath->GetClosestDistanceOnPath(StartLocation);

	UE_LOG(LogMileZero, Log, TEXT("MZTrafficVehicle %s spawned on path %s at distance %.0f"),
		*GetName(), *CurrentPath->GetName(), DistanceAlongPath);
}

void AMZTrafficVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentPath || !CurrentPath->IsPathValid())
	{
		return;
	}

	FollowPath(DeltaTime);
}

void AMZTrafficVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMZTrafficVehicle::SetPath(AMZTrafficPath* NewPath)
{
	CurrentPath = NewPath;
	if (CurrentPath)
	{
		DistanceAlongPath = CurrentPath->GetClosestDistanceOnPath(GetActorLocation());
		bIsStopped = false;
		bIsYielding = false;
	}
}

bool AMZTrafficVehicle::IsObstacleAhead() const
{
	if (!CurrentPath)
	{
		return false;
	}

	// Get current position and direction
	FVector CurrentLocation = GetActorLocation();
	FVector ForwardDirection = GetActorForwardVector();

	// Multi-point raycast across vehicle width
	bool bFoundObstacle = false;

	float HalfWidth = ObstacleCheckWidth * 0.5f;
	int32 NumRays = 3;

	for (int32 i = 0; i < NumRays; ++i)
	{
		float WidthOffset = (i - (NumRays - 1) * 0.5f) * HalfWidth / NumRays;
		FVector RayStart = CurrentLocation + (GetActorRightVector() * WidthOffset);
		FVector RayEnd = RayStart + (ForwardDirection * ObstacleCheckDistance);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.TraceTag = TEXT("TrafficObstacleCheck");

		if (GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_WorldStatic, QueryParams))
		{
			bFoundObstacle = true;
			break;
		}

		// Also check for other traffic vehicles
		if (GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_Pawn, QueryParams))
		{
			if (HitResult.GetActor() && HitResult.GetActor()->IsA<AMZTrafficVehicle>())
			{
				bFoundObstacle = true;
				break;
			}
		}
	}

	return bFoundObstacle;
}

void AMZTrafficVehicle::Despawn()
{
	Destroy();
}

void AMZTrafficVehicle::FollowPath(float DeltaTime)
{
	float PathLength = CurrentPath->GetPathLength();
	if (PathLength <= 0.0f)
	{
		return;
	}

	// Check for obstacles
	bool bObstacleNow = IsObstacleAhead();

	// Desired speed (km/h to cm/s conversion: km/h * 27.777...)
	float TargetSpeedCmPerSec = DesiredSpeed * 27.777f;

	// Handle obstacle avoidance
	if (bObstacleNow)
	{
		// Hard brake to stop
		CurrentSpeed = FMath::Lerp(CurrentSpeed, 0.0f, AccelerationRate * DeltaTime);
		bIsStopped = true;
	}
	else
	{
		// Accelerate to desired speed
		CurrentSpeed = FMath::Lerp(CurrentSpeed, TargetSpeedCmPerSec, AccelerationRate * DeltaTime);
		bIsStopped = false;
	}

	// Advance along path
	float DistanceDelta = CurrentSpeed * DeltaTime;
	DistanceAlongPath += DistanceDelta;

	// Wrap around if at end of path
	if (DistanceAlongPath >= PathLength)
	{
		// Check for connected paths
		if (CurrentPath->ConnectedPaths.Num() > 0)
		{
			AdvanceToNextPath();
		}
		else
		{
			// Loop the current path
			DistanceAlongPath = FMath::Fmod(DistanceAlongPath, PathLength);
		}
	}

	// Update actor position and rotation based on path
	FVector NewLocation = CurrentPath->GetLocationAtDistance(DistanceAlongPath);
	FVector PathDirection = CurrentPath->GetDirectionAtDistance(DistanceAlongPath);
	FRotator NewRotation = PathDirection.Rotation();

	SetActorLocationAndRotation(NewLocation, NewRotation, false);

	// Smooth velocity update for movement component (not used but kept for reference)
	if (MovementComponent)
	{
		MovementComponent->Velocity = PathDirection * CurrentSpeed;
	}
}

void AMZTrafficVehicle::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
	{
		return;
	}

	// Check if hit by player vehicle
	if (OtherActor->IsA<APawn>() && OtherActor != this)
	{
		// Player hit this traffic vehicle
		OnPlayerCollision.Broadcast();
		UE_LOG(LogMileZero, Log, TEXT("Traffic vehicle %s hit by player"), *GetName());
	}
}

void AMZTrafficVehicle::AdvanceToNextPath()
{
	if (!CurrentPath || CurrentPath->ConnectedPaths.Num() == 0)
	{
		return;
	}

	// Pick a random connected path
	int32 NextPathIndex = FMath::RandRange(0, CurrentPath->ConnectedPaths.Num() - 1);
	AMZTrafficPath* NextPath = CurrentPath->ConnectedPaths[NextPathIndex];

	if (NextPath && NextPath->IsPathValid())
	{
		SetPath(NextPath);
		UE_LOG(LogMileZero, Log, TEXT("Traffic vehicle switched to path %s"), *NextPath->GetName());
	}
}
