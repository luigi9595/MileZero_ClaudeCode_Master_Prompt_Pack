#include "MZWorldZone.h"
#include "MileZero.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AMZWorldZone::AMZWorldZone()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	// Root component is the box bounds
	ZoneBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBounds"));
	RootComponent = ZoneBounds;

	// Zone bounds are triggers for detection
	ZoneBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneBounds->SetCollisionObjectType(ECC_WorldStatic);
	ZoneBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ZoneBounds->SetGenerateOverlapEvents(true);

	// Default box size: 10000 x 10000 x 500 (depth in Z)
	ZoneBounds->SetBoxExtent(FVector(5000.0f, 5000.0f, 250.0f));
}

void AMZWorldZone::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap events
	if (ZoneBounds)
	{
		ZoneBounds->OnComponentBeginOverlap.AddDynamic(this, &AMZWorldZone::OnBoxBeginOverlap);
		ZoneBounds->OnComponentEndOverlap.AddDynamic(this, &AMZWorldZone::OnBoxEndOverlap);
	}

	UE_LOG(LogMileZero, Display, TEXT("Zone [%s] initialized: %s (%d spawn points, %d connections)"),
	       *ZoneID.ToString(), *ZoneName.ToString(), SpawnPoints.Num(), ConnectedZones.Num());
}

bool AMZWorldZone::IsLocationInZone(const FVector& Location) const
{
	if (!ZoneBounds)
	{
		return false;
	}

	return ZoneBounds->IsOverlappingActor(GetWorld()->SpawnActor<AActor>(Location, FRotator::ZeroRotator));
}

FVector AMZWorldZone::GetNearestSpawnPoint(const FVector& Location) const
{
	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Zone [%s] has no spawn points"), *ZoneID.ToString());
		return FVector::ZeroVector;
	}

	float NearestDistance = MAX_FLT;
	FVector NearestPoint = SpawnPoints[0];

	for (const FVector& SpawnPoint : SpawnPoints)
	{
		float Distance = FVector::Distance(Location, SpawnPoint);
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestPoint = SpawnPoint;
		}
	}

	return NearestPoint;
}

FVector AMZWorldZone::GetRandomSpawnPoint() const
{
	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Zone [%s] has no spawn points"), *ZoneID.ToString());
		return FVector::ZeroVector;
	}

	int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	return SpawnPoints[RandomIndex];
}

void AMZWorldZone::AddConnectedZone(AMZWorldZone* OtherZone)
{
	if (!OtherZone)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Zone [%s]: Attempted to add null connected zone"), *ZoneID.ToString());
		return;
	}

	if (!ConnectedZones.Contains(OtherZone))
	{
		ConnectedZones.Add(OtherZone);
		UE_LOG(LogMileZero, Display, TEXT("Zone [%s] connected to [%s]"), *ZoneID.ToString(), *OtherZone->ZoneID.ToString());
	}
}

FString AMZWorldZone::GetZoneInfo() const
{
	return FString::Printf(TEXT("Zone: %s | Type: %d | SpeedLimit: %.1f km/h | Surface: %s | Spawns: %d | Connected: %d"),
	                        *ZoneName.ToString(), (int32)ZoneType, DefaultSpeedLimit, *DefaultSurfaceType.ToString(),
	                        SpawnPoints.Num(), ConnectedZones.Num());
}

void AMZWorldZone::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                      bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	// Check if the entering actor is a Pawn (vehicle)
	APawn* VehiclePawn = Cast<APawn>(OtherActor);
	if (VehiclePawn)
	{
		OnVehicleEntered.Broadcast(this, VehiclePawn);
		UE_LOG(LogMileZero, Verbose, TEXT("Vehicle entered zone [%s]"), *ZoneID.ToString());
	}
}

void AMZWorldZone::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	// Check if the exiting actor is a Pawn (vehicle)
	APawn* VehiclePawn = Cast<APawn>(OtherActor);
	if (VehiclePawn)
	{
		OnVehicleExited.Broadcast(this, VehiclePawn);
		UE_LOG(LogMileZero, Verbose, TEXT("Vehicle exited zone [%s]"), *ZoneID.ToString());
	}
}
