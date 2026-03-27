#include "MZHUD.h"
#include "MZHUDWidget.h"
#include "MileZero.h"
#include "Engine/GameViewportClient.h"

AMZHUD::AMZHUD()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMZHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine && GEngine->GameViewport)
	{
		HUDWidget = SNew(SMZHUDWidget);
		GEngine->GameViewport->AddViewportWidgetContent(HUDWidget.ToSharedRef());
		UE_LOG(LogMileZero, Log, TEXT("MZHUD: gameplay HUD widget added to viewport"));
	}
}

void AMZHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HUDWidget.IsValid() && GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(HUDWidget.ToSharedRef());
	}
	HUDWidget.Reset();
	Super::EndPlay(EndPlayReason);
}
