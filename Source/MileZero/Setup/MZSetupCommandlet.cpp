#include "MZSetupCommandlet.h"

#if WITH_EDITOR

#include "MileZero.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"

int32 UMZSetupCommandlet::Main(const FString& Params)
{
	UE_LOG(LogMileZero, Display, TEXT("=== MileZero Setup Commandlet ==="));

	if (CreateTestLevel())
	{
		UE_LOG(LogMileZero, Display, TEXT("Test level created successfully"));
	}
	else
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to create test level"));
		return 1;
	}

	UE_LOG(LogMileZero, Display, TEXT("=== Setup complete ==="));
	return 0;
}

bool UMZSetupCommandlet::CreateTestLevel()
{
	const FString PackageName = TEXT("/Game/MileZero/Maps/Test/L_MZ_TestTrack");

	// Check if level already exists
	FString ExistingPath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());
	if (FPaths::FileExists(ExistingPath))
	{
		UE_LOG(LogMileZero, Display, TEXT("Test level already exists at %s — skipping"), *ExistingPath);
		return true;
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

	// --- Directional light (sun) ---
	{
		ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(FVector::ZeroVector, FRotator(-45.0f, -45.0f, 0.0f), SpawnParams);
		if (Sun)
		{
			Sun->SetActorLabel(TEXT("Sun"));
			Sun->GetComponent()->SetIntensity(6.0f);
			UE_LOG(LogMileZero, Display, TEXT("  + Directional light (sun)"));
		}
	}

	// --- Sky light for ambient ---
	{
		ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector(0.0f, 0.0f, 500.0f), FRotator::ZeroRotator, SpawnParams);
		if (Sky)
		{
			Sky->SetActorLabel(TEXT("SkyLight"));
			Sky->GetLightComponent()->SetIntensity(1.0f);
			Sky->GetLightComponent()->bRealTimeCapture = true;
			UE_LOG(LogMileZero, Display, TEXT("  + Sky light (ambient)"));
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

#endif // WITH_EDITOR
