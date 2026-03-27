#include "MZDebugSubsystem.h"
#include "MileZero.h"
#include "MileZero/Vehicles/MZVehiclePawn.h"
#include "MileZero/Vehicles/MZVehicleRegistry.h"
#include "MileZero/Vehicles/MZVehicleDataAsset.h"
#include "MileZero/UI/MZTelemetryWidget.h"
#include "UI/MZVehicleSelectionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"
#include "Engine/GameViewportClient.h"

void UMZDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Register console commands (Exec doesn't work on subsystems)
	RegisteredCommands.Add(
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("MZ.ResetVehicle"),
			TEXT("Reset the current vehicle to its spawn point"),
			FConsoleCommandDelegate::CreateLambda([this]()
			{
				ResetCurrentVehicle();
			}),
			ECVF_Default
		)
	);

	RegisteredCommands.Add(
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("MZ.SpawnVehicle"),
			TEXT("Spawn a vehicle by ID (e.g., MZ.SpawnVehicle Hatch)"),
			FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args)
			{
				if (Args.Num() > 0)
				{
					SpawnVehicleByID(Args[0]);
				}
				else
				{
					UE_LOG(LogMileZero, Warning, TEXT("Usage: MZ.SpawnVehicle <VehicleID>"));
				}
			}),
			ECVF_Default
		)
	);

	RegisteredCommands.Add(
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("MZ.Teleport"),
			TEXT("Teleport player to X Y Z (e.g., MZ.Teleport 0 0 200)"),
			FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args)
			{
				if (Args.Num() >= 3)
				{
					TeleportTo(FCString::Atof(*Args[0]), FCString::Atof(*Args[1]), FCString::Atof(*Args[2]));
				}
				else
				{
					UE_LOG(LogMileZero, Warning, TEXT("Usage: MZ.Teleport X Y Z"));
				}
			}),
			ECVF_Default
		)
	);

	RegisteredCommands.Add(
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("MZ.Telemetry"),
			TEXT("Toggle telemetry overlay"),
			FConsoleCommandDelegate::CreateLambda([this]()
			{
				ToggleTelemetryOverlay();
			}),
			ECVF_Default
		)
	);

	RegisteredCommands.Add(
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("MZ.SelectVehicle"),
			TEXT("Toggle vehicle selection widget"),
			FConsoleCommandDelegate::CreateLambda([this]()
			{
				ToggleVehicleSelectionWidget();
			}),
			ECVF_Default
		)
	);

	RegisteredCommands.Add(
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("MZ.ListVehicles"),
			TEXT("List all registered vehicles with their stats"),
			FConsoleCommandDelegate::CreateLambda([this]()
			{
				UGameInstance* GI = GetGameInstance();
				if (GI)
				{
					if (UMZVehicleRegistry* Registry = GI->GetSubsystem<UMZVehicleRegistry>())
					{
						TArray<FName> VehicleIDs = Registry->GetAllVehicleIDs();
						UE_LOG(LogMileZero, Log, TEXT("=== Registered Vehicles ==="));
						for (const FName& ID : VehicleIDs)
						{
							if (UMZVehicleDataAsset* Data = Registry->GetVehicleData(ID))
							{
								const char* DrivetrainStr = (Data->DrivetrainType == EMZDrivetrainType::RWD) ? "RWD" :
														   (Data->DrivetrainType == EMZDrivetrainType::FWD) ? "FWD" : "AWD";
								UE_LOG(LogMileZero, Log,
									TEXT("  %s: %.0f kg, %s, %.0f Nm, %.0f RPM, max steer %.0f°"),
									*ID.ToString(),
									Data->MassKg,
									ANSI_TO_TCHAR(DrivetrainStr),
									Data->Engine.MaxTorque,
									Data->Engine.MaxRPM,
									Data->MaxSteerAngle);
							}
						}
					}
				}
			}),
			ECVF_Default
		)
	);

	UE_LOG(LogMileZero, Log, TEXT("MZDebugSubsystem initialized — console commands registered (MZ.*)"));
}

void UMZDebugSubsystem::Deinitialize()
{
	// Remove telemetry widget if active
	if (TelemetryWidget.IsValid() && GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(TelemetryWidget.ToSharedRef());
		TelemetryWidget.Reset();
	}

	// Remove vehicle selection widget if active
	if (VehicleSelectionWidget.IsValid() && GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(VehicleSelectionWidget.ToSharedRef());
		VehicleSelectionWidget.Reset();
	}

	for (IConsoleObject* Cmd : RegisteredCommands)
	{
		IConsoleManager::Get().UnregisterConsoleObject(Cmd);
	}
	RegisteredCommands.Empty();

	Super::Deinitialize();
}

void UMZDebugSubsystem::ToggleTelemetryOverlay()
{
	bTelemetryVisible = !bTelemetryVisible;

	if (bTelemetryVisible)
	{
		// Create and show telemetry widget
		if (!TelemetryWidget.IsValid() && GEngine && GEngine->GameViewport)
		{
			TelemetryWidget = SNew(SMZTelemetryWidget);
			GEngine->GameViewport->AddViewportWidgetContent(TelemetryWidget.ToSharedRef());
			UE_LOG(LogMileZero, Log, TEXT("Telemetry overlay: ON"));
		}
	}
	else
	{
		// Hide and destroy telemetry widget
		if (TelemetryWidget.IsValid() && GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(TelemetryWidget.ToSharedRef());
			TelemetryWidget.Reset();
			UE_LOG(LogMileZero, Log, TEXT("Telemetry overlay: OFF"));
		}
	}
}

void UMZDebugSubsystem::SpawnVehicleByID(const FString& VehicleID)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No player controller available"));
		return;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No game instance available"));
		return;
	}

	UMZVehicleRegistry* Registry = GI->GetSubsystem<UMZVehicleRegistry>();
	if (!Registry)
	{
		UE_LOG(LogMileZero, Warning, TEXT("MZVehicleRegistry not found"));
		return;
	}

	FName VehicleID_Name = FName(*VehicleID);
	UMZVehicleDataAsset* VehicleData = Registry->GetVehicleData(VehicleID_Name);
	if (!VehicleData)
	{
		UE_LOG(LogMileZero, Warning, TEXT("Vehicle not found: %s"), *VehicleID);
		return;
	}

	// Get current vehicle location and rotation
	AMZVehiclePawn* CurrentVehicle = Cast<AMZVehiclePawn>(PC->GetPawn());
	FTransform SpawnTransform = FTransform::Identity;
	if (CurrentVehicle)
	{
		SpawnTransform = CurrentVehicle->GetActorTransform();
		CurrentVehicle->Destroy();
	}

	// Spawn new vehicle
	AMZVehiclePawn* NewVehicle = GetWorld()->SpawnActor<AMZVehiclePawn>(
		AMZVehiclePawn::StaticClass(),
		SpawnTransform
	);

	if (NewVehicle)
	{
		NewVehicle->ApplyVehicleData(VehicleData);
		PC->Possess(NewVehicle);
		UE_LOG(LogMileZero, Log, TEXT("Spawned vehicle: %s"), *VehicleID);
	}
	else
	{
		UE_LOG(LogMileZero, Error, TEXT("Failed to spawn vehicle: %s"), *VehicleID);
	}
}

void UMZDebugSubsystem::ToggleVehicleSelectionWidget()
{
	bVehicleSelectionVisible = !bVehicleSelectionVisible;

	if (bVehicleSelectionVisible)
	{
		if (!VehicleSelectionWidget.IsValid() && GEngine && GEngine->GameViewport)
		{
			VehicleSelectionWidget = SNew(SMZVehicleSelectionWidget).DebugSubsystem(this);
			GEngine->GameViewport->AddViewportWidgetContent(VehicleSelectionWidget.ToSharedRef());
			UE_LOG(LogMileZero, Log, TEXT("Vehicle selection widget: ON"));
		}
	}
	else
	{
		if (VehicleSelectionWidget.IsValid() && GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(VehicleSelectionWidget.ToSharedRef());
			VehicleSelectionWidget.Reset();
			UE_LOG(LogMileZero, Log, TEXT("Vehicle selection widget: OFF"));
		}
	}
}

void UMZDebugSubsystem::ResetCurrentVehicle()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		if (AMZVehiclePawn* Vehicle = Cast<AMZVehiclePawn>(PC->GetPawn()))
		{
			Vehicle->ResetVehicle();
		}
	}
}

void UMZDebugSubsystem::TeleportTo(float X, float Y, float Z)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->GetPawn())
	{
		PC->GetPawn()->SetActorLocation(FVector(X, Y, Z), false, nullptr, ETeleportType::ResetPhysics);
		UE_LOG(LogMileZero, Log, TEXT("Teleported to (%.0f, %.0f, %.0f)"), X, Y, Z);
	}
}
