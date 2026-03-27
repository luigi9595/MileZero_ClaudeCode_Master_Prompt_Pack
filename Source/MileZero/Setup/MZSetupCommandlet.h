#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR

#include "Commandlets/Commandlet.h"
#include "MZSetupCommandlet.generated.h"

class AMZWorldZone;
class AMZSpawnPoint;

/**
 * Commandlet to auto-generate essential MileZero assets and worlds:
 * - L_MZ_TestTrack level with floor and PlayerStart (legacy)
 * - L_MZ_MainWorld level with full greybox zones and infrastructure
 *
 * Run via: UnrealEditor-Cmd.exe MileZero.uproject -run=MZSetup
 */
UCLASS()
class UMZSetupCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	virtual int32 Main(const FString& Params) override;

private:
	bool CreateTestLevel();
	bool CreateMainWorld();

	// Helper methods for main world creation
	void CreateZone(UWorld* World, const FVector& ZoneCenter, const FVector& ZoneExtent,
	                 const FName& ZoneID, const FText& ZoneName, int32 ZoneTypeIndex,
	                 float SpeedLimit, const FName& SurfaceType, const FActorSpawnParameters& SpawnParams);

	void CreateSpawnPointsInZone(UWorld* World, const FVector& ZoneCenter, const FVector& ZoneExtent,
	                               const FName& ZoneID, int32 Count, bool bIsDefault,
	                               const FActorSpawnParameters& SpawnParams);

	void CreateGroundPlane(UWorld* World, const FVector& Location, const FVector& Scale,
	                        const FString& Label, const FActorSpawnParameters& SpawnParams);

	void ConnectAdjacentZones(AMZWorldZone* Zone1, AMZWorldZone* Zone2);
};

#endif // WITH_EDITOR
