// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TacticalShooter/Public/SpawnPoint/TeamDeathmatchSpawnPoint.h"
#include "TacticalShooter/Public/PlayerState/TeamDeathmatchPlayerState.h"
#include "TeamDeathmatchGameMode.generated.h"

UCLASS()
class TACTICALSHOOTER_API ATeamDeathmatchGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void SpawnPlayer(APlayerController* PC);


	UPROPERTY(EditAnywhere)
		int32 KillsLimit;

	UPROPERTY(EditAnywhere)
		int32 ScoreLimit;

	UPROPERTY(EditAnywhere)
		int32 TimeLimit;

	UPROPERTY(EditAnywhere)
		int32 ScorePerKill = 0;

	UPROPERTY(EditAnywhere)
		int32 SecondsBetweenDeathAndSpawn = 0;

	UPROPERTY(EditAnywhere)
		int32 SecondsBetweenMatches = 0;

	virtual void AddKill(ATeamDeathmatchPlayerState* PS);

	inline virtual int32 GetSecondsBetweenDeathAndSpawn() { return SecondsBetweenDeathAndSpawn; };
	inline virtual int32 GetSecondsBetweenBetweenMatches() { return SecondsBetweenMatches; };

	FTimerHandle RestartMatchTimerHandle;
	
protected:
	bool bHasLoadedSpawnPoints = false;
	ETeam LastJoinedPlayerTeam = ETeam::TeamB;

	TArray<ATeamDeathmatchSpawnPoint*> PlayerSpawnPoints;

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleMatchHasEnded() override;

	virtual void LoadSpawnPoints();
	virtual void EndGame();
};
