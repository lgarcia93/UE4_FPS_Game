// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "CustomGameInstance.h"
#include "GameOnlineBeaconHostObject.generated.h"


/**
 * 
 */
UCLASS()
class TACTICALSHOOTER_API AGameOnlineBeaconHostObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()

public:
	AGameOnlineBeaconHostObject();
protected:
	FLobbyInfo LobbyInfo;
protected:
	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;
	virtual void NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor) override;

	UFUNCTION(BlueprintCallable)
		void ShutdownServer();

	void DisconnectAllClients();

	virtual void DisconnectClient(AOnlineBeaconClient* ClientActor) override;

	UFUNCTION(BlueprintCallable)
		void UpdateMapInfo(FMapInfo MapInfo);

	void UpdateLobbyInfoOnClients();

	UFUNCTION(BlueprintCallable)
		void StartServer(FString MapURL);
	
};
