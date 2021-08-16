// Copyright Epic Games, Inc. All Rights Reserved.

#include "TacticalShooterGameMode.h"
#include "TacticalShooterHUD.h"
#include "TacticalShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "OnlineBeaconHost.h"
#include "GameOnlineBeaconHostObject.h"

ATacticalShooterGameMode::ATacticalShooterGameMode()
	: Super()
{		
	// use our custom HUD class
	HUDClass = ATacticalShooterHUD::StaticClass();
}


bool ATacticalShooterGameMode::CreateHostBeacon()
{
	Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass());

	if (Host) {
		if (Host->InitHost()) {
			Host->PauseBeaconRequests(false);

			HostObject = GetWorld()->SpawnActor<AGameOnlineBeaconHostObject>(AGameOnlineBeaconHostObject::StaticClass());

			if (HostObject) {
				Host->RegisterHost(HostObject);

				return true;
			}
		}
	}	
	return false;
}

class AGameOnlineBeaconHostObject* ATacticalShooterGameMode::GetHostObject()
{
	return HostObject;
}