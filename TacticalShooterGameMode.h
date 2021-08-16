// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TacticalShooterGameMode.generated.h"

UCLASS(minimalapi)
class ATacticalShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
		bool CreateHostBeacon();

	class AGameOnlineBeaconHostObject* HostObject;
	class AOnlineBeaconHost* Host;

	UFUNCTION(BlueprintCallable)
		class AGameOnlineBeaconHostObject* GetHostObject();

public:
	ATacticalShooterGameMode();
};



