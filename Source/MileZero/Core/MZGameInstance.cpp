#include "MZGameInstance.h"
#include "MileZero.h"

void UMZGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogMileZero, Log, TEXT("MZGameInstance initialized — default vehicle: %s"), *SelectedVehicleID.ToString());
}
