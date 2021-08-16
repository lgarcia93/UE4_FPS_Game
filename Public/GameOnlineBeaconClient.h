// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "TacticalShooter/Public/CustomGameInstance.h"
#include "GameOnlineBeaconClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectedDelegate, bool, OnConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLobbyUpdatedDelegate, FLobbyInfo, OnLobbyUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisconnectedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFullConnectDelegate);
/**
 * 
 */
UCLASS()
class TACTICALSHOOTER_API AGameOnlineBeaconClient : public AOnlineBeaconClient
{
	GENERATED_BODY()
	
public:
	AGameOnlineBeaconClient();

protected:
	UPROPERTY(BlueprintAssignable)
		FConnectedDelegate FOnConnected;
	UPROPERTY(BlueprintAssignable)
		FDisconnectedDelegate FOnDisconnected;

	UPROPERTY(BlueprintAssignable)
		FLobbyUpdatedDelegate OnLobbyUpdated;

	UPROPERTY(BlueprintAssignable)
		FFullConnectDelegate OnFullConnected;

protected:
	UFUNCTION(BlueprintCallable)
		bool ConnectToServer(const FString& Address);

	UFUNCTION(BlueprintCallable)
		void FullConnectToServer(const FString& Address);

	UFUNCTION(BlueprintCallable)
		void LeaveLobby();

	virtual void OnFailure() override;
	virtual void OnConnected() override;

public:
	UFUNCTION(Client, Reliable)
		void Client_OnDisconnected();

	UFUNCTION(Client, Reliable)
		void Client_OnLobbyUpdated(FLobbyInfo LobbyInfo);

	UFUNCTION(Client, Reliable)
		void Client_OnFullConnect();

	virtual void Client_OnDisconnected_Implementation();

	virtual void Client_OnLobbyUpdated_Implementation(FLobbyInfo LobbyInfo);

	virtual void Client_OnFullConnect_Implementation();

};
