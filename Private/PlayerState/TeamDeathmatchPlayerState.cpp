// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/PlayerState/TeamDeathmatchPlayerState.h"
#include "Engine/World.h"
//#include "TacticalShooter/Public/GameState/TeamDeathmatchGameState.h"
#include "TacticalShooter/Public/CustomGameInstance.h"
#include "Net/UnrealNetwork.h"

void ATeamDeathmatchPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamDeathmatchPlayerState, bIsDead);
	DOREPLIFETIME(ATeamDeathmatchPlayerState, PlayerCurrentScore);
	DOREPLIFETIME(ATeamDeathmatchPlayerState, PlayerKillCount);
	DOREPLIFETIME(ATeamDeathmatchPlayerState, Team);
}

bool ATeamDeathmatchPlayerState::GetIsDead()
{
	return bIsDead;
}

void ATeamDeathmatchPlayerState::SetIsDead(bool IsDead)
{
	bIsDead = IsDead;
}

int32 ATeamDeathmatchPlayerState::GetCurrentScore()
{
	return PlayerCurrentScore;
}

int32 ATeamDeathmatchPlayerState::GetPlayerKillCount()
{
	return PlayerKillCount;
}

void ATeamDeathmatchPlayerState::IncreasePlayerKillCount()
{
	PlayerKillCount++;
}

void ATeamDeathmatchPlayerState::IncreaseScore(int32 Points)
{
	PlayerCurrentScore += Points;
}

ETeam ATeamDeathmatchPlayerState::GetTeam()
{
	return Team;
}

void ATeamDeathmatchPlayerState::SetTeam(ETeam NewTeam)
{
	Team = NewTeam;
}

void ATeamDeathmatchPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}