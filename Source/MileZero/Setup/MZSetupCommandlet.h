#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR

#include "Commandlets/Commandlet.h"
#include "MZSetupCommandlet.generated.h"

/**
 * Commandlet to auto-generate essential MileZero assets:
 * - L_MZ_TestTrack level with floor and PlayerStart
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
};

#endif // WITH_EDITOR
