// Fill out your copyright notice in the Description page of Project Settings.

// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/GameMode/TeamDeathmatchGameMode.h"
#include "TacticalShooter/Public/GameState/TeamDeathmatchGameState.h"
#include "TacticalShooter/Public/PlayerState/TeamDeathmatchPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TacticalShooter/Public/SpawnPoint/TeamDeathmatchSpawnPoint.h"
#include "Math/UnrealMathUtility.h"


void ATeamDeathmatchGameMode::BeginPlay()
{

}

void ATeamDeathmatchGameMode::HandleMatchHasEnded()
{

}

void ATeamDeathmatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!bHasLoadedSpawnPoints)
	{
		LoadSpawnPoints();
	}

	for (auto SpawnPoint : PlayerSpawnPoints)
	{
		//Use even used spawn points with true || bypass				
		if (true || !SpawnPoint->GetIsUsed())
		{
			FVector SpawnLocation = SpawnPoint->GetActorLocation();

			if (UWorld* World = GetWorld())
			{
				if (APawn* Pawn = World->SpawnActor<APawn>(DefaultPawnClass, SpawnLocation, FRotator::ZeroRotator))
				{
					NewPlayer->Possess(Pawn);

					SpawnPoint->SetIsUsed();

					ATeamDeathmatchPlayerState* PS = NewPlayer->GetPlayerState<ATeamDeathmatchPlayerState>();

					GetGameState<ATeamDeathmatchGameState>()->AddPlayerState(PS);

					ETeam NewPlayerTeam = LastJoinedPlayerTeam == ETeam::TeamA ? ETeam::TeamB : ETeam::TeamA;

					PS->SetTeam(NewPlayerTeam);

					LastJoinedPlayerTeam = NewPlayerTeam;

					PS->SetIsDead(false);

					return;
				}
			}
		}
	}
}

void ATeamDeathmatchGameMode::SpawnPlayer(APlayerController* PC)
{
	ATeamDeathmatchSpawnPoint* SpawnPoint = PlayerSpawnPoints[FMath::RandRange(0, PlayerSpawnPoints.Num() - 1)];

	FVector SpawnLocation = SpawnPoint->GetActorLocation();

	if (UWorld* World = GetWorld())
	{
		if (APawn* Pawn = World->SpawnActor<APawn>(DefaultPawnClass, SpawnLocation, FRotator::ZeroRotator))
		{
			PC->UnPossess();

			ATeamDeathmatchPlayerState* PS = PC->GetPlayerState<ATeamDeathmatchPlayerState>();

			PS->SetIsDead(false);

			PC->Possess(Pawn);

			return;
		}
	}
}

void ATeamDeathmatchGameMode::LoadSpawnPoints()
{
	TArray<AActor*> TempActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamDeathmatchSpawnPoint::StaticClass(), TempActors);

	if (TempActors.Num())
	{
		for (AActor* Actor : TempActors)
		{
			if (ATeamDeathmatchSpawnPoint* SpawnPoint = Cast<ATeamDeathmatchSpawnPoint>(Actor))
			{
				PlayerSpawnPoints.Add(SpawnPoint);
			}
		}
	}

	bHasLoadedSpawnPoints = true;
}

void ATeamDeathmatchGameMode::AddKill(ATeamDeathmatchPlayerState* PS)
{
	bool ShouldRestartMatch = false;

	if (ATeamDeathmatchGameState* GS = GetGameState<ATeamDeathmatchGameState>())
	{
		GS->IncreaseKillCount(PS->GetTeam(), ScorePerKill);

		PS->IncreaseScore(ScorePerKill);

		if (GS->GetTeamAKillCount() >= KillsLimit || GS->GetTeamAScore() >= ScoreLimit)
		{
			GS->SetWinner(ETeam::TeamA);
						
			EndMatch();

			ShouldRestartMatch = true;
		}

		if (GS->GetTeamBKillCount() >= KillsLimit || GS->GetTeamBScore() >= ScoreLimit)
		{
			GS->SetWinner(ETeam::TeamB);

			EndMatch();

			ShouldRestartMatch = true;
		}
	}

	if (ShouldRestartMatch) {
		//EndGame();

		GetWorld()->GetTimerManager().SetTimer(
			RestartMatchTimerHandle,
			this,
			&ATeamDeathmatchGameMode::EndGame,
			SecondsBetweenMatches,
			false
		);
	}
}

void ATeamDeathmatchGameMode::EndGame()
{
	RestartGame();

	GetWorld()->GetTimerManager().ClearTimer(RestartMatchTimerHandle);
}
