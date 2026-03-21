#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZDebugSubsystem.generated.h"

UCLASS()
class MILEZERO_API UMZDebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "MZ|Debug")
	void ToggleTelemetryOverlay();

	UFUNCTION(BlueprintPure, Category = "MZ|Debug")
	bool IsTelemetryVisible() const { return bTelemetryVisible; }

	void SpawnVehicleByID(const FString& VehicleID);
	void ResetCurrentVehicle();
	void TeleportTo(float X, float Y, float Z);

private:
	bool bTelemetryVisible = false;

	// Console command handles for cleanup
	TArray<IConsoleObject*> RegisteredCommands;
};
