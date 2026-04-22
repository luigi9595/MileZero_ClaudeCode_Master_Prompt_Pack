#include "MZPedestrianActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "../MileZero.h"

AMZPedestrianActor::AMZPedestrianActor()
	: CurrentState(EMZPedestrianState::Idle)
	, bIsRunning(false)
	, DistanceToTarget(0.0f)
	, FleeingTimer(0.0f)
	, bIsFleeing(false)
	, FleeDirection(FVector::ZeroVector)
{
	PrimaryActorTick.TickInterval = 0.1f;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// Actor setup
	SetCanBeDamaged(true);

	// Root component: static mesh as capsule placeholder
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	RootComponent = VisualMesh;
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	VisualMesh->SetCollisionObjectType(ECC_Pawn);
	VisualMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	VisualMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	VisualMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Optional skeletal mesh for when art exists
	PedestrianMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PedestrianMesh"));
	PedestrianMesh->SetupAttachment(RootComponent);
	PedestrianMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMZPedestrianActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialize random target location
	PickNewTargetLocation();

	// Start idle
	CurrentState = EMZPedestrianState::Idle;
	bIsFleeing = false;
	FleeingTimer = 0.0f;

	UE_LOG(LogMileZero, Log, TEXT("Pedestrian spawned at %s"), *GetActorLocation().ToString());
}

void AMZPedestrianActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetWorld())
	{
		return;
	}

	// Update fleeing timer
	if (bIsFleeing)
	{
		FleeingTimer += DeltaTime;
		if (FleeingTimer >= FleeingStateTimeout)
		{
			bIsFleeing = false;
			FleeingTimer = 0.0f;
		}
	}

	// Update movement toward target
	UpdateMovement(DeltaTime);

	// Check for obstacles and adjust direction
	CheckObstaclesAndAvoid();

	// Update internal state machine
	UpdateState();

	// Face the direction we're moving
	FaceMovementDirection();
}

void AMZPedestrianActor::UpdateMovement(float DeltaTime)
{
	// Determine current speed based on state and fleeing flag
	float CurrentSpeed = bIsRunning ? RunSpeed : WalkSpeed;
	if (bIsFleeing)
	{
		CurrentSpeed = RunSpeed;
	}

	// Get current position
	FVector CurrentLocation = GetActorLocation();

	// Calculate direction to target
	FVector DirectionToTarget = (TargetLocation - CurrentLocation);
	DistanceToTarget = DirectionToTarget.Length();

	// If reached target, pick a new one (unless fleeing)
	if (DistanceToTarget < 150.0f)
	{
		if (!bIsFleeing)
		{
			PickNewTargetLocation();
		}
		else
		{
			// While fleeing, keep moving in the flee direction
			DirectionToTarget = FleeDirection;
		}
	}

	// Normalize and apply speed
	if (DistanceToTarget > 0.1f)
	{
		DirectionToTarget.Normalize();
	}
	else
	{
		DirectionToTarget = FVector::ZeroVector;
	}

	FVector NewLocation = CurrentLocation + (DirectionToTarget * CurrentSpeed * DeltaTime);
	SetActorLocation(NewLocation, true);
}

void AMZPedestrianActor::PickNewTargetLocation()
{
	if (!GetWorld())
	{
		return;
	}

	// Pick random point within 500-1500cm radius
	float RandomDistance = FMath::RandRange(500.0f, 1500.0f);
	float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);

	FVector CurrentLocation = GetActorLocation();
	FVector Offset = FVector(
		FMath::Cos(RandomAngle) * RandomDistance,
		FMath::Sin(RandomAngle) * RandomDistance,
		0.0f
	);

	TargetLocation = CurrentLocation + Offset;

	// Simple height adjustment (clamp to ground plane)
	TargetLocation.Z = CurrentLocation.Z;

	// Random chance to walk vs run
	bIsRunning = FMath::RandBool();
}

void AMZPedestrianActor::CheckObstaclesAndAvoid()
{
	if (!GetWorld())
	{
		return;
	}

	FVector ActorLocation = GetActorLocation();
	FVector DirectionToTarget = (TargetLocation - ActorLocation).GetSafeNormal();

	// Raycast forward to check for obstacles
	FVector RayStart = ActorLocation + FVector(0.0f, 0.0f, 50.0f);
	FVector RayEnd = RayStart + (DirectionToTarget * ObstacleCheckDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_WorldStatic, QueryParams);

	if (bHit && HitResult.GetActor())
	{
		// Obstacle detected - pick new target
		PickNewTargetLocation();
	}
}

void AMZPedestrianActor::UpdateState()
{
	// Update state machine based on movement and fleeing
	if (bIsFleeing)
	{
		CurrentState = EMZPedestrianState::Fleeing;
	}
	else if (bIsRunning)
	{
		CurrentState = EMZPedestrianState::Running;
	}
	else if (DistanceToTarget > 100.0f)
	{
		CurrentState = EMZPedestrianState::Walking;
	}
	else
	{
		CurrentState = EMZPedestrianState::Idle;
	}
}

void AMZPedestrianActor::FaceMovementDirection()
{
	FVector DirectionToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();

	if (DirectionToTarget.Length() > 0.1f)
	{
		FRotator TargetRotation = DirectionToTarget.Rotation();
		FRotator CurrentRotation = GetActorRotation();

		// Smooth rotation
		FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, 0.1f);
		SetActorRotation(NewRotation);
	}
}

void AMZPedestrianActor::FleeFromLocation(FVector DangerSource)
{
	if (!GetWorld())
	{
		return;
	}

	// Calculate flee direction (away from danger)
	FVector ActorLocation = GetActorLocation();
	FleeDirection = (ActorLocation - DangerSource).GetSafeNormal();

	// Set new target in flee direction
	TargetLocation = ActorLocation + (FleeDirection * 2000.0f);

	// Activate fleeing state
	bIsFleeing = true;
	FleeingTimer = 0.0f;
	bIsRunning = true;

	UE_LOG(LogMileZero, Log, TEXT("Pedestrian fleeing from %s"), *DangerSource.ToString());
}

void AMZPedestrianActor::OnHitByVehicle(FVector ImpactLocation)
{
	if (!GetWorld())
	{
		return;
	}

	// Option 1: Flee from impact
	FleeFromLocation(ImpactLocation);

	// Option 2: Could also despawn and respawn elsewhere (disabled for now)
	// Destroy();
	// UE_LOG(LogMileZero, Log, TEXT("Pedestrian hit by vehicle, despawned"));
}
