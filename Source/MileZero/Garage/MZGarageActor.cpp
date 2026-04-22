#include "MZGarageActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Vehicles/MZVehiclePawn.h"
#include "Economy/MZEconomySubsystem.h"
#include "MileZero.h"
#include "Damage/MZVehicleDamageComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogMZGarage, Log, All);

AMZGarageActor::AMZGarageActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	// Root component: box collision for trigger volume
	GarageZone = CreateDefaultSubobject<UBoxComponent>(TEXT("GarageZone"));
	RootComponent = GarageZone;
	GarageZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GarageZone->SetCollisionObjectType(ECC_WorldDynamic);
	GarageZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	GarageZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GarageZone->SetBoxExtent(FVector(500.0f, 500.0f, 300.0f));

	// Optional visual mesh for the garage structure
	GarageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GarageMesh"));
	GarageMesh->SetupAttachment(RootComponent);
	GarageMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GarageMesh->SetCollisionObjectType(ECC_WorldStatic);
	GarageMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	ParkedVehicle = nullptr;

	// Initialize default paint colors
	InitializeDefaultColors();
}

void AMZGarageActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap delegates
	if (GarageZone)
	{
		GarageZone->OnComponentBeginOverlap.AddDynamic(this, &AMZGarageActor::OnOverlapBegin);
		GarageZone->OnComponentEndOverlap.AddDynamic(this, &AMZGarageActor::OnOverlapEnd);
	}

	UE_LOG(LogMileZero, Log, TEXT("Garage '%s' initialized at %s"), *GarageID.ToString(), *GetActorLocation().ToString());
}

float AMZGarageActor::GetRepairCost(AMZVehiclePawn* Vehicle) const
{
	if (!Vehicle)
	{
		return 0.0f;
	}

	// Get damage percentage (0-100)
	float DamagePercent = Vehicle->GetDamagePercent();
	float Cost = DamagePercent * RepairCostPerPercent;

	return Cost;
}

bool AMZGarageActor::CanAffordRepair(AMZVehiclePawn* Vehicle) const
{
	if (!Vehicle)
	{
		return false;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return false;
	}

	UMZEconomySubsystem* EconomySubsystem = GI->GetSubsystem<UMZEconomySubsystem>();
	if (!EconomySubsystem)
	{
		return false;
	}

	float Cost = GetRepairCost(Vehicle);
	return EconomySubsystem->CanAfford(Cost);
}

bool AMZGarageActor::RepairVehicle(AMZVehiclePawn* Vehicle)
{
	if (!Vehicle)
	{
		UE_LOG(LogMileZero, Warning, TEXT("RepairVehicle called with null vehicle"));
		return false;
	}

	float Cost = GetRepairCost(Vehicle);

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return false;
	}

	UMZEconomySubsystem* EconomySubsystem = GI->GetSubsystem<UMZEconomySubsystem>();
	if (!EconomySubsystem)
	{
		return false;
	}

	// Check if we can afford it
	if (!EconomySubsystem->CanAfford(Cost))
	{
		UE_LOG(LogMileZero, Log, TEXT("Cannot afford repair: cost $%.2f, have $%.2f"),
			Cost, EconomySubsystem->GetCurrentMoney());
		return false;
	}

	// Spend money
	FString RepairReason = FString::Printf(TEXT("Repair at %s"), *GarageName.ToString());
	if (!EconomySubsystem->SpendMoney(Cost, RepairReason))
	{
		return false;
	}

	// Repair the vehicle
	if (Vehicle->GetDamageComponent())
	{
		Vehicle->GetDamageComponent()->RepairAll();
		UE_LOG(LogMileZero, Log, TEXT("Vehicle repaired at '%s' for $%.2f"), *GarageID.ToString(), Cost);
		return true;
	}

	return false;
}

bool AMZGarageActor::PaintVehicle(AMZVehiclePawn* Vehicle, FLinearColor Color)
{
	if (!Vehicle)
	{
		return false;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return false;
	}

	UMZEconomySubsystem* EconomySubsystem = GI->GetSubsystem<UMZEconomySubsystem>();
	if (!EconomySubsystem)
	{
		return false;
	}

	// Check if we can afford it
	if (!EconomySubsystem->CanAfford(PaintCostPerColor))
	{
		UE_LOG(LogMileZero, Log, TEXT("Cannot afford paint job: cost $%.2f"), PaintCostPerColor);
		return false;
	}

	// Spend money for paint
	FString PaintReason = FString::Printf(TEXT("Paint job at %s"), *GarageName.ToString());
	if (!EconomySubsystem->SpendMoney(PaintCostPerColor, PaintReason))
	{
		return false;
	}

	// Apply the color to the vehicle
	// This would apply to the vehicle's material(s)
	// For now, we log the action; actual material modification depends on vehicle setup
	Vehicle->SetActorRelativeScale3D(FVector(1.0f)); // Placeholder - real code would apply material

	UE_LOG(LogMileZero, Log, TEXT("Vehicle painted at '%s' with color (R=%.2f, G=%.2f, B=%.2f, A=%.2f) for $%.2f"),
		*GarageID.ToString(), Color.R, Color.G, Color.B, Color.A, PaintCostPerColor);

	return true;
}

void AMZGarageActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AMZVehiclePawn* Vehicle = Cast<AMZVehiclePawn>(OtherActor);
	if (Vehicle)
	{
		ParkedVehicle = Vehicle;
		OnVehicleGarageStateChanged.Broadcast(true);
		UE_LOG(LogMileZero, Log, TEXT("Vehicle entered garage '%s'"), *GarageID.ToString());
	}
}

void AMZGarageActor::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AMZVehiclePawn* Vehicle = Cast<AMZVehiclePawn>(OtherActor);
	if (Vehicle && Vehicle == ParkedVehicle)
	{
		ParkedVehicle = nullptr;
		OnVehicleGarageStateChanged.Broadcast(false);
		UE_LOG(LogMileZero, Log, TEXT("Vehicle left garage '%s'"), *GarageID.ToString());
	}
}

void AMZGarageActor::InitializeDefaultColors()
{
	if (AvailableColors.Num() == 0)
	{
		// Add default paint color options
		AvailableColors.Add(FLinearColor::Red);       // Red
		AvailableColors.Add(FLinearColor::Blue);      // Blue
		AvailableColors.Add(FLinearColor::Green);     // Green
		AvailableColors.Add(FLinearColor::White);     // White
		AvailableColors.Add(FLinearColor::Black);     // Black
		AvailableColors.Add(FLinearColor::Yellow);    // Yellow
		AvailableColors.Add(FLinearColor(0.5f, 0.5f, 0.5f)); // Gray
		AvailableColors.Add(FLinearColor(1.0f, 0.5f, 0.0f)); // Orange
	}
}
