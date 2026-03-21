#include "MZDebugSubsystem.h"
#include "MileZero.h"
#include "MileZero/Vehicles/MZVehiclePawn.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"

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

	UE_LOG(LogMileZero, Log, TEXT("MZDebugSubsystem initialized — console commands registered (MZ.*)"));
}

void UMZDebugSubsystem::Deinitialize()
{
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
	UE_LOG(LogMileZero, Log, TEXT("Telemetry overlay: %s"), bTelemetryVisible ? TEXT("ON") : TEXT("OFF"));
}

void UMZDebugSubsystem::SpawnVehicleByID(const FString& VehicleID)
{
	UE_LOG(LogMileZero, Log, TEXT("MZ.SpawnVehicle: %s — wired when vehicle data registry exists"), *VehicleID);
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
