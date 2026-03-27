#include "MZSetupCommandlet.h"

#if WITH_EDITOR

#include "MileZero.h"
#include "World/MZWorldZone.h"
#include "World/MZSpawnPoint.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Lightmass/LightmassImportanceVolume.h"
#include "Components/BrushComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"

int32 UMZSetupCommandlet::Main(const FString& Params)
{
	UE_LOG(LogMileZero, Display, TEXT("=== MileZero Setup Commandlet ==="));

	// Create test level (legacy)
	if (CreateTestLevel())
	{
		UE_LOG(LogMileZero, Display, TEXT("Test level created successfully"));
	}
	else
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to create test level"));
		return 1;
	}

	// Create main world with greybox zones
	if (CreateMainWorld())
	{
		UE_LOG(LogMileZero, Display, TEXT("Main world created successfully"));
	}
	else
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to create main world"));
		return 1;
	}

	UE_LOG(LogMileZero, Display, TEXT("=== Setup complete ==="));
	return 0;
}

bool UMZSetupCommandlet::CreateTestLevel()
{
	const FString PackageName = TEXT("/Game/MileZero/Maps/Test/L_MZ_TestTrack");

	// Delete old level if it exists so we can regenerate
	FString ExistingPath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());
	if (FPaths::FileExists(ExistingPath))
	{
		UE_LOG(LogMileZero, Display, TEXT("Removing old test level: %s"), *ExistingPath);
		IFileManager::Get().Delete(*ExistingPath);
	}

	UE_LOG(LogMileZero, Display, TEXT("Creating test level: %s"), *PackageName);

	// Create the package
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to create package"));
		return false;
	}
	Package->FullyLoad();

	// Create the world
	UWorld* World = UWorld::CreateWorld(EWorldType::None, false, TEXT("L_MZ_TestTrack"), Package);
	if (!World)
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to create world"));
		return false;
	}

	World->SetFlags(RF_Public | RF_Standalone);
	Package->SetDirtyFlag(true);

	// Initialize the world for actor spawning
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::None);
	WorldContext.SetCurrentWorld(World);
	World->InitWorld();
	World->PersistentLevel->OwningWorld = World;

	FActorSpawnParameters SpawnParams;
	SpawnParams.OverrideLevel = World->PersistentLevel;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// --- Large floor plane ---
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh)
	{
		AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Floor)
		{
			Floor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
			Floor->SetActorScale3D(FVector(200.0f, 200.0f, 1.0f));
			Floor->SetActorLabel(TEXT("Floor"));
			UE_LOG(LogMileZero, Display, TEXT("  + Floor (200x200 scale plane)"));
		}
	}
	else
	{
		UE_LOG(LogMileZero, Warning, TEXT("  Could not load /Engine/BasicShapes/Plane — floor not created"));
	}

	// --- PlayerStart 50cm above floor ---
	{
		APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(FVector(0.0f, 0.0f, 50.0f), FRotator::ZeroRotator, SpawnParams);
		if (PlayerStart)
		{
			PlayerStart->SetActorLabel(TEXT("PlayerStart_Vehicle"));
			UE_LOG(LogMileZero, Display, TEXT("  + PlayerStart at (0, 0, 50)"));
		}
	}

	// --- Directional light (sun) with atmosphere interaction ---
	{
		ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(FVector::ZeroVector, FRotator(-45.0f, -45.0f, 0.0f), SpawnParams);
		if (Sun)
		{
			Sun->SetActorLabel(TEXT("Sun"));
			Sun->GetComponent()->SetIntensity(6.0f);
			Sun->GetComponent()->SetAtmosphereSunLight(true);
			Sun->GetComponent()->SetLightingChannels(true, false, false);
			UE_LOG(LogMileZero, Display, TEXT("  + Directional light (sun, atmosphere-linked)"));
		}
	}

	// --- Sky Atmosphere (required for SkyLight real-time capture) ---
	{
		ASkyAtmosphere* Atmo = World->SpawnActor<ASkyAtmosphere>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Atmo)
		{
			Atmo->SetActorLabel(TEXT("SkyAtmosphere"));
			UE_LOG(LogMileZero, Display, TEXT("  + SkyAtmosphere"));
		}
	}

	// --- Sky light for ambient (now valid with SkyAtmosphere) ---
	{
		ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector(0.0f, 0.0f, 500.0f), FRotator::ZeroRotator, SpawnParams);
		if (Sky)
		{
			Sky->SetActorLabel(TEXT("SkyLight"));
			Sky->GetLightComponent()->SetIntensity(1.0f);
			Sky->GetLightComponent()->bRealTimeCapture = true;
			Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
			UE_LOG(LogMileZero, Display, TEXT("  + Sky light (real-time capture)"));
		}
	}

	// --- Exponential height fog for atmosphere ---
	{
		AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(FVector(0.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
		if (Fog)
		{
			Fog->SetActorLabel(TEXT("HeightFog"));
			Fog->GetComponent()->SetFogDensity(0.01f);
			Fog->GetComponent()->SetFogHeightFalloff(0.2f);
			UE_LOG(LogMileZero, Display, TEXT("  + Exponential height fog"));
		}
	}

	// --- Lightmass Importance Volume ---
	{
		ALightmassImportanceVolume* LIV = World->SpawnActor<ALightmassImportanceVolume>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (LIV)
		{
			// Cover the driving area: 200m x 200m x 20m (floor is 200-scale plane = 20000x20000 UU)
			LIV->SetActorLabel(TEXT("LightmassImportanceVolume"));
			LIV->SetActorScale3D(FVector(400.0f, 400.0f, 40.0f));
			UE_LOG(LogMileZero, Display, TEXT("  + Lightmass Importance Volume"));
		}
	}

	// Save the level
	FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());

	// Ensure parent directory exists
	FString Directory = FPaths::GetPath(FilePath);
	IFileManager::Get().MakeDirectory(*Directory, true);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	bool bSaved = UPackage::SavePackage(Package, World, *FilePath, SaveArgs);

	// Cleanup world context
	GEngine->DestroyWorldContext(World);

	if (bSaved)
	{
		UE_LOG(LogMileZero, Display, TEXT("  Level saved to: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogMileZero, Error, TEXT("  Failed to save level to: %s"), *FilePath);
	}

	return bSaved;
}

bool UMZSetupCommandlet::CreateMainWorld()
{
	const FString PackageName = TEXT("/Game/MileZero/Maps/Main/L_MZ_MainWorld");

	// Delete old level if it exists
	FString ExistingPath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());
	if (FPaths::FileExists(ExistingPath))
	{
		UE_LOG(LogMileZero, Display, TEXT("Removing old main world: %s"), *ExistingPath);
		IFileManager::Get().Delete(*ExistingPath);
	}

	UE_LOG(LogMileZero, Display, TEXT("Creating main world: %s"), *PackageName);

	// Create the package and world
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to create package"));
		return false;
	}
	Package->FullyLoad();

	UWorld* World = UWorld::CreateWorld(EWorldType::None, false, TEXT("L_MZ_MainWorld"), Package);
	if (!World)
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to create world"));
		return false;
	}

	World->SetFlags(RF_Public | RF_Standalone);
	Package->SetDirtyFlag(true);

	// Initialize the world for actor spawning
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::None);
	WorldContext.SetCurrentWorld(World);
	World->InitWorld();
	World->PersistentLevel->OwningWorld = World;

	FActorSpawnParameters SpawnParams;
	SpawnParams.OverrideLevel = World->PersistentLevel;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UE_LOG(LogMileZero, Display, TEXT("  Creating zones..."));

	// Zone layout:
	// City: center (0, 0)
	// Industrial: east (15000, 0)
	// ProvingGround: west (-15000, 0)
	// Highway: long strip connecting north-south
	// Rural: south (0, 15000)

	// City district (center, 10000x10000)
	CreateZone(World, FVector(0.0f, 0.0f, 0.0f), FVector(5000.0f, 5000.0f, 250.0f),
	           TEXT("City"), FText::FromString(TEXT("City District")), 0,
	           50.0f, TEXT("DryAsphalt"), SpawnParams);

	// Industrial zone (east, 8000x8000)
	CreateZone(World, FVector(15000.0f, 0.0f, 0.0f), FVector(4000.0f, 4000.0f, 250.0f),
	           TEXT("Industrial"), FText::FromString(TEXT("Industrial Zone")), 1,
	           60.0f, TEXT("DryAsphalt"), SpawnParams);

	// Highway/Ring-road (long strip connecting zones)
	CreateZone(World, FVector(7500.0f, 0.0f, 0.0f), FVector(7500.0f, 1000.0f, 250.0f),
	           TEXT("Highway"), FText::FromString(TEXT("Highway / Ring Road")), 2,
	           100.0f, TEXT("DryAsphalt"), SpawnParams);

	// Rural/Gravel spur (south, 12000x12000)
	CreateZone(World, FVector(0.0f, 15000.0f, 0.0f), FVector(6000.0f, 6000.0f, 250.0f),
	           TEXT("Rural"), FText::FromString(TEXT("Rural / Gravel")), 3,
	           70.0f, TEXT("Gravel"), SpawnParams);

	// Proving ground (west, 6000x6000)
	CreateZone(World, FVector(-15000.0f, 0.0f, 0.0f), FVector(3000.0f, 3000.0f, 250.0f),
	           TEXT("ProvingGround"), FText::FromString(TEXT("Proving Ground")), 4,
	           120.0f, TEXT("DryAsphalt"), SpawnParams);

	// Create spawn points in each zone
	UE_LOG(LogMileZero, Display, TEXT("  Creating spawn points..."));
	CreateSpawnPointsInZone(World, FVector(0.0f, 0.0f, 0.0f), FVector(5000.0f, 5000.0f, 250.0f),
	                         TEXT("City"), 3, false, SpawnParams);
	CreateSpawnPointsInZone(World, FVector(15000.0f, 0.0f, 0.0f), FVector(4000.0f, 4000.0f, 250.0f),
	                         TEXT("Industrial"), 2, false, SpawnParams);
	CreateSpawnPointsInZone(World, FVector(0.0f, 15000.0f, 0.0f), FVector(6000.0f, 6000.0f, 250.0f),
	                         TEXT("Rural"), 2, false, SpawnParams);

	// Default spawn in proving ground
	CreateSpawnPointsInZone(World, FVector(-15000.0f, 0.0f, 0.0f), FVector(3000.0f, 3000.0f, 250.0f),
	                         TEXT("ProvingGround"), 1, true, SpawnParams);

	// Create ground planes
	UE_LOG(LogMileZero, Display, TEXT("  Creating ground planes..."));
	CreateGroundPlane(World, FVector(0.0f, 0.0f, -100.0f), FVector(50.0f, 50.0f, 1.0f),
	                   TEXT("Ground_City"), SpawnParams);
	CreateGroundPlane(World, FVector(15000.0f, 0.0f, -100.0f), FVector(40.0f, 40.0f, 1.0f),
	                   TEXT("Ground_Industrial"), SpawnParams);
	CreateGroundPlane(World, FVector(0.0f, 15000.0f, -100.0f), FVector(60.0f, 60.0f, 1.0f),
	                   TEXT("Ground_Rural"), SpawnParams);
	CreateGroundPlane(World, FVector(-15000.0f, 0.0f, -100.0f), FVector(30.0f, 30.0f, 1.0f),
	                   TEXT("Ground_ProvingGround"), SpawnParams);
	CreateGroundPlane(World, FVector(7500.0f, 0.0f, -100.0f), FVector(75.0f, 10.0f, 1.0f),
	                   TEXT("Ground_Highway"), SpawnParams);

	// Create player start at proving ground
	UE_LOG(LogMileZero, Display, TEXT("  Creating PlayerStart..."));
	{
		APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(
		    FVector(-15000.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
		if (PlayerStart)
		{
			PlayerStart->SetActorLabel(TEXT("PlayerStart_Main"));
			UE_LOG(LogMileZero, Display, TEXT("    + PlayerStart at Proving Ground"));
		}
	}

	// Create environment actors
	UE_LOG(LogMileZero, Display, TEXT("  Creating environment..."));

	// Directional light (sun)
	{
		ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(
		    FVector::ZeroVector, FRotator(-45.0f, -45.0f, 0.0f), SpawnParams);
		if (Sun)
		{
			Sun->SetActorLabel(TEXT("Sun"));
			Sun->GetComponent()->SetIntensity(6.0f);
			Sun->GetComponent()->SetAtmosphereSunLight(true);
			Sun->GetComponent()->SetLightingChannels(true, false, false);
			UE_LOG(LogMileZero, Display, TEXT("    + Directional light"));
		}
	}

	// Sky atmosphere
	{
		ASkyAtmosphere* Atmo = World->SpawnActor<ASkyAtmosphere>(
		    FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Atmo)
		{
			Atmo->SetActorLabel(TEXT("SkyAtmosphere"));
			UE_LOG(LogMileZero, Display, TEXT("    + Sky atmosphere"));
		}
	}

	// Sky light
	{
		ASkyLight* Sky = World->SpawnActor<ASkyLight>(
		    FVector(0.0f, 0.0f, 500.0f), FRotator::ZeroRotator, SpawnParams);
		if (Sky)
		{
			Sky->SetActorLabel(TEXT("SkyLight"));
			Sky->GetLightComponent()->SetIntensity(1.0f);
			Sky->GetLightComponent()->bRealTimeCapture = true;
			Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
			UE_LOG(LogMileZero, Display, TEXT("    + Sky light"));
		}
	}

	// Height fog
	{
		AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(
		    FVector(0.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
		if (Fog)
		{
			Fog->SetActorLabel(TEXT("HeightFog"));
			Fog->GetComponent()->SetFogDensity(0.01f);
			Fog->GetComponent()->SetFogHeightFalloff(0.2f);
			UE_LOG(LogMileZero, Display, TEXT("    + Height fog"));
		}
	}

	// Lightmass importance volume
	{
		ALightmassImportanceVolume* LIV = World->SpawnActor<ALightmassImportanceVolume>(
		    FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (LIV)
		{
			LIV->SetActorLabel(TEXT("LightmassImportanceVolume"));
			LIV->SetActorScale3D(FVector(800.0f, 800.0f, 80.0f));
			UE_LOG(LogMileZero, Display, TEXT("    + Lightmass importance volume"));
		}
	}

	// Save the world
	FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());
	FString Directory = FPaths::GetPath(FilePath);
	IFileManager::Get().MakeDirectory(*Directory, true);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	bool bSaved = UPackage::SavePackage(Package, World, *FilePath, SaveArgs);

	GEngine->DestroyWorldContext(World);

	if (bSaved)
	{
		UE_LOG(LogMileZero, Display, TEXT("  Main world saved to: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogMileZero, Error, TEXT("  Failed to save main world to: %s"), *FilePath);
	}

	return bSaved;
}

void UMZSetupCommandlet::CreateZone(UWorld* World, const FVector& ZoneCenter, const FVector& ZoneExtent,
                                     const FName& ZoneID, const FText& ZoneName, int32 ZoneTypeIndex,
                                     float SpeedLimit, const FName& SurfaceType,
                                     const FActorSpawnParameters& SpawnParams)
{
	if (!World)
	{
		return;
	}

	AMZWorldZone* Zone = World->SpawnActor<AMZWorldZone>(ZoneCenter, FRotator::ZeroRotator, SpawnParams);
	if (!Zone)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Failed to spawn zone [%s]"), *ZoneID.ToString());
		return;
	}

	Zone->SetActorLabel(FString::Printf(TEXT("Zone_%s"), *ZoneID.ToString()));
	Zone->ZoneID = ZoneID;
	Zone->ZoneName = ZoneName;
	Zone->ZoneType = (EMZZoneType)ZoneTypeIndex;
	Zone->DefaultSpeedLimit = SpeedLimit;
	Zone->DefaultSurfaceType = SurfaceType;

	// Set zone bounds size
	if (Zone->ZoneBounds)
	{
		Zone->ZoneBounds->SetBoxExtent(ZoneExtent);
	}

	UE_LOG(LogMileZero, Display, TEXT("    + Zone created: %s (%s)"), *ZoneID.ToString(), *ZoneName.ToString());
}

void UMZSetupCommandlet::CreateSpawnPointsInZone(UWorld* World, const FVector& ZoneCenter,
                                                  const FVector& ZoneExtent, const FName& ZoneID,
                                                  int32 Count, bool bIsDefault,
                                                  const FActorSpawnParameters& SpawnParams)
{
	if (!World || Count <= 0)
	{
		return;
	}

	for (int32 i = 0; i < Count; ++i)
	{
		// Random position within zone
		FVector RandomOffset(
		    FMath::RandRange(-ZoneExtent.X * 0.8f, ZoneExtent.X * 0.8f),
		    FMath::RandRange(-ZoneExtent.Y * 0.8f, ZoneExtent.Y * 0.8f),
		    100.0f);

		FVector SpawnLocation = ZoneCenter + RandomOffset;

		AMZSpawnPoint* SpawnPoint = World->SpawnActor<AMZSpawnPoint>(SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (!SpawnPoint)
		{
			continue;
		}

		SpawnPoint->SetActorLabel(FString::Printf(TEXT("SpawnPoint_%s_%d"), *ZoneID.ToString(), i));
		SpawnPoint->AssociatedZoneID = ZoneID;
		SpawnPoint->SpawnLocation = SpawnLocation;
		SpawnPoint->SpawnRotation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
		SpawnPoint->bIsDefaultSpawn = bIsDefault && (i == 0);
		SpawnPoint->bIsRecoveryPoint = true;
	}

	UE_LOG(LogMileZero, Display, TEXT("    + Created %d spawn points in zone [%s]"), Count, *ZoneID.ToString());
}

void UMZSetupCommandlet::CreateGroundPlane(UWorld* World, const FVector& Location, const FVector& Scale,
                                            const FString& Label, const FActorSpawnParameters& SpawnParams)
{
	if (!World)
	{
		return;
	}

	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (!PlaneMesh)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Could not load plane mesh for ground"));
		return;
	}

	AStaticMeshActor* Ground = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator, SpawnParams);
	if (!Ground)
	{
		return;
	}

	Ground->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
	Ground->SetActorScale3D(Scale);
	Ground->SetActorLabel(Label);
}

void UMZSetupCommandlet::ConnectAdjacentZones(AMZWorldZone* Zone1, AMZWorldZone* Zone2)
{
	if (Zone1 && Zone2)
	{
		Zone1->AddConnectedZone(Zone2);
		Zone2->AddConnectedZone(Zone1);
	}
}

#endif // WITH_EDITOR
