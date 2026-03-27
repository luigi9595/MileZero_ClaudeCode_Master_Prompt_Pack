#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MZDebugSubsystem.generated.h"

class SMZTelemetryWidget;
class SMZVehicleSelectionWidget;
class UMZVehicleRegistry;

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

	UFUNCTION(BlueprintCallable, Category = "MZ|Debug")
	void ToggleVehicleSelectionWidget();

	UFUNCTION(BlueprintPure, Category = "MZ|Debug")
	bool IsVehicleSelectionVisible() const { return bVehicleSelectionVisible; }

	void SpawnVehicleByID(const FString& VehicleID);
	void ResetCurrentVehicle();
	void TeleportTo(float X, float Y, float Z);

private:
	bool bTelemetryVisible = false;
	bool bVehicleSelectionVisible = false;

	TSharedPtr<SMZTelemetryWidget> TelemetryWidget;
	TSharedPtr<SMZVehicleSelectionWidget> VehicleSelectionWidget;

	// Console command handles for cleanup
	TArray<IConsoleObject*> RegisteredCommands;
};
