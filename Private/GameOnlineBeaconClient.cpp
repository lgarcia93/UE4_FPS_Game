// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/GameOnlineBeaconClient.h"
#include "TacticalShooter/Public/CustomGameInstance.h"

AGameOnlineBeaconClient::AGameOnlineBeaconClient ()
{

}

bool AGameOnlineBeaconClient::ConnectToServer(const FString& Address) {
	FURL Destination = FURL(
		nullptr,
		*Address,
		ETravelType::TRAVEL_Absolute
	);

	Destination.Port = 7787;

	return InitClient(Destination);
}

void AGameOnlineBeaconClient::FullConnectToServer(const FString& Address)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController()) {
		PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void AGameOnlineBeaconClient::OnFailure()
{
	FOnConnected.Broadcast(false);
}

void AGameOnlineBeaconClient::OnConnected()
{	
	FOnConnected.Broadcast(true);
}

void AGameOnlineBeaconClient::Client_OnDisconnected_Implementation()
{	
	FOnDisconnected.Broadcast();
}

void AGameOnlineBeaconClient::LeaveLobby()
{
	DestroyBeacon();
}

void AGameOnlineBeaconClient::Client_OnLobbyUpdated_Implementation(FLobbyInfo LobbyInfo)
{	
	OnLobbyUpdated.Broadcast(LobbyInfo);	
}

void AGameOnlineBeaconClient::Client_OnFullConnect_Implementation()
{
	OnFullConnected.Broadcast();
	LeaveLobby();
}
