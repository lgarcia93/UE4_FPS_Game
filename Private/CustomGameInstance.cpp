// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/CustomGameInstance.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Engine/World.h"


UCustomGameInstance::UCustomGameInstance()
{
	//Init();
}
//
//void UCustomGameInstance::GoToMap(FString mapUrl)
//{
//	GetWorld()->ServerTravel(mapUrl);
//}
//
//void UCustomGameInstance::Init()
//{
//	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
//	{
//		SessionInterface = SubSystem->GetSessionInterface();
//
//		if (SessionInterface.IsValid())
//		{
//			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCustomGameInstance::OnCreateSessionComplete);
//			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCustomGameInstance::OnFindSessionsComplete);
//			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCustomGameInstance::OnJoinSessionsComplete);
//		}
//	}
//}
//
//void UCustomGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
//{
//	if (Succeeded)
//	{
//		if (UWorld* World = GetWorld())
//		{
//			World->ServerTravel("/Game/Maps/GameMap?listen", ETravelType::TRAVEL_Absolute);
//		}
//	}
//}
//
//void UCustomGameInstance::OnFindSessionsComplete(bool Succeeded)
//{
//	if (Succeeded)
//	{
//		if (SessionSearch.IsValid())
//		{
//			TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
//
//			if (Results.Num())
//			{
//				for (auto& Result : Results)
//				{
//					FServerInfo ServerInfo;
//
//					FString ServerName;
//
//					Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName);
//
//					ServerInfo.ServerName = ServerName;
//					ServerInfo.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
//					ServerInfo.CurrentPlayers = Result.Session.NumOpenPublicConnections;
//					ServerInfo.Ping = Result.PingInMs;
//
//					OnSessionFound.Broadcast(ServerInfo);
//				}
//			}
//		}
//	}
//}
//
//void UCustomGameInstance::OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
//{
//	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
//	{
//		FString JoinAddress;
//		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
//
//		if (JoinAddress != "")
//			PC->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
//	}
//}
//
//void UCustomGameInstance::CreateSession()
//{
//	FOnlineSessionSettings OnlineSessionSettings;
//
//	OnlineSessionSettings.bAllowJoinInProgress = true;
//	OnlineSessionSettings.bIsDedicated = true;
//	OnlineSessionSettings.bIsLANMatch = false;
//	OnlineSessionSettings.bShouldAdvertise = true;
//	OnlineSessionSettings.bUsesPresence = true;
//	OnlineSessionSettings.NumPublicConnections = 5;
//
//	FString ServerName = "TacticalShooter";
//	FString Hostname = "TacticalShooter";
//
//	OnlineSessionSettings.Set(
//		FName("SERVER_NAME_KEY"),
//		ServerName,
//		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
//	);
//
//	OnlineSessionSettings.Set(
//		FName("SERVER_HOSTNAME_KEY"),
//		Hostname,
//		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
//	);
//
//	SessionInterface->CreateSession(0, FName("Game"), OnlineSessionSettings);
//}
//
//void UCustomGameInstance::FindSessions()
//{
//	SessionSearch = MakeShareable(new FOnlineSessionSearch());
//
//	SessionSearch->bIsLanQuery = false;
//	SessionSearch->MaxSearchResults = 100000;
//	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
//
//	if (SessionSearch.IsValid())
//		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
//}
//
//void UCustomGameInstance::JoinSession(int32 SessionIndex)
//{
//	SessionInterface->JoinSession(0, FName("Game"), SessionSearch->SearchResults[SessionIndex]);
//}
