#pragma once

#include "CoreMinimal.h"
#include "MZRadioTypes.generated.h"

class USoundBase;

/**
 * Single radio station definition
 */
USTRUCT(BlueprintType)
struct FMZRadioStation
{
	GENERATED_BODY()

	/** Unique identifier for this station (e.g., "MZ_Rock") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Radio")
	FName StationID;

	/** Display name for this station (e.g., "Mile Rock") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Radio")
	FText StationName;

	/** Genre description (e.g., "Rock", "Electronic", "Jazz") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Radio")
	FText Genre;

	/** Track IDs/names currently in this station's playlist */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Radio")
	TArray<FName> TrackIDs;

	/** Current track index in playlist */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|Radio")
	int32 CurrentTrackIndex = 0;

	/** Audio sources for each track (populated when assets exist) */
	UPROPERTY(BlueprintReadOnly, Category = "MZ|Radio")
	TArray<TObjectPtr<USoundBase>> TrackAudio;

	FMZRadioStation()
		: StationID(FName())
		, StationName(FText())
		, Genre(FText())
		, CurrentTrackIndex(0)
	{
	}

	FMZRadioStation(FName InID, FText InName, FText InGenre)
		: StationID(InID)
		, StationName(InName)
		, Genre(InGenre)
		, CurrentTrackIndex(0)
	{
	}
};
