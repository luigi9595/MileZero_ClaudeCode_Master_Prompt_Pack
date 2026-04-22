#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MZHUD.generated.h"

class SMZHUDWidget;
class SMZMinimapWidget;
class SMZNotificationWidget;

/**
 * Main HUD manager. Owns and manages the gameplay HUD widget lifecycle.
 */
UCLASS()
class MILEZERO_API AMZHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMZHUD();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TSharedPtr<SMZHUDWidget> HUDWidget;
	TSharedPtr<SMZMinimapWidget> MinimapWidget;
	TSharedPtr<SMZNotificationWidget> NotificationWidget;
};
