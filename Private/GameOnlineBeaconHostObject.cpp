// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/GameOnlineBeaconHostObject.h"
#include "OnlineBeaconHost.h"
#include "GameOnlineBeaconClient.h"
#include "TacticalShooter/Public/GameOnlineBeaconHostObject.h"
#include "TacticalShooter/Public/CustomGameInstance.h"

AGameOnlineBeaconHostObject::AGameOnlineBeaconHostObject()
{
	ClientBeaconActorClass = AGameOnlineBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();
}

void AGameOnlineBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (NewClientActor) {

		if (AGameOnlineBeaconClient* Client = Cast<AGameOnlineBeaconClient>(NewClientActor))
		{
			FString PlayerName = "Player";

			PlayerName.Append(FString::FromInt(LobbyInfo.Players.Num() + 1));

			LobbyInfo.Players.Add(PlayerName);
			UpdateLobbyInfoOnClients();
			Client->Client_OnLobbyUpdated(LobbyInfo);		
		}
	}
}

void AGameOnlineBeaconHostObject::NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor)
{
	Super::NotifyClientDisconnected(LeavingClientActor);
}

void AGameOnlineBeaconHostObject::ShutdownServer() {

	DisconnectAllClients();

	if (AOnlineBeaconHost* Host = Cast<AOnlineBeaconHost>(GetOwner()))
	{
		Host->UnregisterHost(BeaconTypeName);
		Host->DestroyBeacon();
	}

	Unregister();
}

void AGameOnlineBeaconHostObject::DisconnectAllClients()
{
	for (auto client : ClientActors) {
		if (client) {
			DisconnectClient(client);
		}
	}
}

void AGameOnlineBeaconHostObject::DisconnectClient(AOnlineBeaconClient* ClientActor)
{
	AOnlineBeaconHost* BeaconHost = Cast<AOnlineBeaconHost>(GetOwner());

	if (BeaconHost) {

		if (AGameOnlineBeaconClient* Client = Cast<AGameOnlineBeaconClient>(ClientActor))
		{
			Client->Client_OnDisconnected();
		}

		BeaconHost->DisconnectClient(ClientActor);
	}
}

void AGameOnlineBeaconHostObject::UpdateMapInfo(FMapInfo MapInfo)
{
	LobbyInfo.MapInfo = MapInfo;

	UpdateLobbyInfoOnClients();

}

void AGameOnlineBeaconHostObject::UpdateLobbyInfoOnClients()
{
	for (auto client : ClientActors) {
		if (AGameOnlineBeaconClient* Client = Cast<AGameOnlineBeaconClient>(client))
		{

			Client->Client_OnLobbyUpdated(LobbyInfo);
		}
	}
}

void AGameOnlineBeaconHostObject::StartServer(FString MapURL) {
	for (auto client : ClientActors) {
		if (AGameOnlineBeaconClient* Client = Cast<AGameOnlineBeaconClient>(client))
		{

			Client->Client_OnFullConnect();
		}
	}

	ShutdownServer();

	GetWorld()->ServerTravel(MapURL + "?listen", ETravelType::TRAVEL_Absolute);
}