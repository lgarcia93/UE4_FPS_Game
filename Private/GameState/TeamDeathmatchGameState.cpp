// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/GameState/TeamDeathmatchGameState.h"
#include "TacticalShooter/Public/PlayerState/TeamDeathmatchPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

void ATeamDeathmatchGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamDeathmatchGameState, TeamAKillCount);
	DOREPLIFETIME(ATeamDeathmatchGameState, TeamBKillCount);
	DOREPLIFETIME(ATeamDeathmatchGameState, TeamAScore);
	DOREPLIFETIME(ATeamDeathmatchGameState, TeamBScore);
	DOREPLIFETIME(ATeamDeathmatchGameState, Winner);
}

int32 ATeamDeathmatchGameState::GetTeamAKillCount()
{
	return TeamAKillCount;
}

int32 ATeamDeathmatchGameState::GetTeamBKillCount()
{
	return TeamBKillCount;
}

int32 ATeamDeathmatchGameState::GetTeamAScore()
{
	return TeamAScore;
}

int32 ATeamDeathmatchGameState::GetTeamBScore()
{
	return TeamBScore;
}

void ATeamDeathmatchGameState::SetWinner(ETeam GameWinner)
{
	Winner = GameWinner;

	OnRep_Winner();
}

void ATeamDeathmatchGameState::HandleMatchHasEnded()
{
	if (Winner == ETeam::TeamA)
	{
		UE_LOG(LogTemp, Warning, TEXT("TeamA is winner"));
	}
	else if (Winner == ETeam::TeamB)
	{
		UE_LOG(LogTemp, Warning, TEXT("TeamB is winner"));
	}
}

void ATeamDeathmatchGameState::PostInitializeComponents() {
	Super::PostInitializeComponents();

	for (TActorIterator<APlayerState> It(GetWorld()); It; ++It) {
		AddPlayerState(*It);
	}
}

void ATeamDeathmatchGameState::OnRep_Winner()
{
	if (Winner != ETeam::None)
		OnMatchEnded.Broadcast();
}

void ATeamDeathmatchGameState::AddPlayerState(APlayerState* PlayerState) {
	Super::AddPlayerState(PlayerState);

	if (!PlayerState->IsInactive()) {
		PlayerArray.AddUnique(PlayerState);
	}
}

void ATeamDeathmatchGameState::IncreaseKillCount(ETeam Team, int32 ScorePerKill)
{
	if (HasAuthority())
	{
		switch (Team)
		{
		case ETeam::TeamA:
			TeamAKillCount++;
			TeamAScore += ScorePerKill;
			break;
		case ETeam::TeamB:
			TeamBKillCount++;
			TeamBScore += ScorePerKill;
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("TeamA kills %d"), TeamAKillCount);
		UE_LOG(LogTemp, Warning, TEXT("TeamB kills %d"), TeamBKillCount);
	}
}