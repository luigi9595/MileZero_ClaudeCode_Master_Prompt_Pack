#include "MZRadioManager.h"
#include "Components/AudioComponent.h"
#include "../MileZero.h"
#include "Engine/World.h"

void UMZRadioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentStationIndex = 0;
	bRadioEnabled = true;

	// Set up default stations
	SetupDefaultStations();

	UE_LOG(LogMileZero, Log, TEXT("MZRadioManager initialized with %d stations"), Stations.Num());
}

void UMZRadioManager::Deinitialize()
{
	if (RadioAudioComponent)
	{
		RadioAudioComponent->Stop();
		RadioAudioComponent = nullptr;
	}

	Stations.Empty();

	UE_LOG(LogMileZero, Log, TEXT("MZRadioManager deinitialized"));

	Super::Deinitialize();
}

void UMZRadioManager::SetupDefaultStations()
{
	Stations.Empty();

	// Station 1: Mile Rock
	FMZRadioStation RockStation(
		FName("MZ_Rock"),
		FText::FromString(TEXT("Mile Rock")),
		FText::FromString(TEXT("Rock"))
	);
	RockStation.TrackIDs = {
		FName("Track_Rock_01"),
		FName("Track_Rock_02"),
		FName("Track_Rock_03")
	};
	Stations.Add(RockStation);

	// Station 2: Zero Wave
	FMZRadioStation ElectronicStation(
		FName("MZ_Electronic"),
		FText::FromString(TEXT("Zero Wave")),
		FText::FromString(TEXT("Electronic"))
	);
	ElectronicStation.TrackIDs = {
		FName("Track_Synth_01"),
		FName("Track_Synth_02"),
		FName("Track_Synth_03")
	};
	Stations.Add(ElectronicStation);

	// Station 3: Smooth Mile
	FMZRadioStation JazzStation(
		FName("MZ_Jazz"),
		FText::FromString(TEXT("Smooth Mile")),
		FText::FromString(TEXT("Jazz / Soul"))
	);
	JazzStation.TrackIDs = {
		FName("Track_Jazz_01"),
		FName("Track_Jazz_02"),
		FName("Track_Jazz_03")
	};
	Stations.Add(JazzStation);

	// Station 4: MZ Classics
	FMZRadioStation ClassicalStation(
		FName("MZ_Classical"),
		FText::FromString(TEXT("MZ Classics")),
		FText::FromString(TEXT("Classical"))
	);
	ClassicalStation.TrackIDs = {
		FName("Track_Classical_01"),
		FName("Track_Classical_02"),
		FName("Track_Classical_03")
	};
	Stations.Add(ClassicalStation);

	// Station 5: Zero Talk Radio
	FMZRadioStation TalkStation(
		FName("MZ_Talk"),
		FText::FromString(TEXT("Zero Talk Radio")),
		FText::FromString(TEXT("Talk"))
	);
	TalkStation.TrackIDs = {
		FName("Track_Talk_01"),
		FName("Track_Talk_02")
	};
	Stations.Add(TalkStation);

	// Station 6: Off
	FMZRadioStation OffStation(
		FName("OFF"),
		FText::FromString(TEXT("Radio Off")),
		FText::FromString(TEXT("---"))
	);
	Stations.Add(OffStation);

	CurrentStationIndex = 0;
}

void UMZRadioManager::NextStation()
{
	if (Stations.IsEmpty())
	{
		return;
	}

	CurrentStationIndex = (CurrentStationIndex + 1) % Stations.Num();

	FMZRadioStation CurrentStation = GetCurrentStation();
	OnStationChanged.Broadcast(CurrentStation, CurrentStationIndex);

	UE_LOG(LogMileZero, Log, TEXT("Radio tuned to %s"), *CurrentStation.StationName.ToString());
}

void UMZRadioManager::PrevStation()
{
	if (Stations.IsEmpty())
	{
		return;
	}

	CurrentStationIndex = (CurrentStationIndex - 1 + Stations.Num()) % Stations.Num();

	FMZRadioStation CurrentStation = GetCurrentStation();
	OnStationChanged.Broadcast(CurrentStation, CurrentStationIndex);

	UE_LOG(LogMileZero, Log, TEXT("Radio tuned to %s"), *CurrentStation.StationName.ToString());
}

FMZRadioStation UMZRadioManager::GetCurrentStation() const
{
	if (Stations.IsValidIndex(CurrentStationIndex))
	{
		return Stations[CurrentStationIndex];
	}

	return FMZRadioStation();
}

FText UMZRadioManager::GetCurrentStationName() const
{
	FMZRadioStation Station = GetCurrentStation();
	return Station.StationName;
}

FText UMZRadioManager::GetCurrentStationGenre() const
{
	FMZRadioStation Station = GetCurrentStation();
	return Station.Genre;
}

void UMZRadioManager::TuneToStation(FName StationID)
{
	for (int32 i = 0; i < Stations.Num(); ++i)
	{
		if (Stations[i].StationID == StationID)
		{
			CurrentStationIndex = i;
			OnStationChanged.Broadcast(Stations[i], i);
			UE_LOG(LogMileZero, Log, TEXT("Radio tuned to %s"), *Stations[i].StationName.ToString());
			return;
		}
	}

	UE_LOG(LogMileZero, Warning, TEXT("Radio station %s not found"), *StationID.ToString());
}

FMZRadioStation UMZRadioManager::GetStationByID(FName StationID) const
{
	for (const FMZRadioStation& Station : Stations)
	{
		if (Station.StationID == StationID)
		{
			return Station;
		}
	}

	return FMZRadioStation();
}

void UMZRadioManager::SetRadioEnabled(bool bEnabled)
{
	bRadioEnabled = bEnabled;

	if (RadioAudioComponent)
	{
		if (bEnabled)
		{
			// Would play here if audio assets exist
		}
		else
		{
			RadioAudioComponent->Stop();
		}
	}

	UE_LOG(LogMileZero, Log, TEXT("Radio %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}
