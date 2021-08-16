// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TacticalShooter/Public/PlayerState/TeamDeathmatchPlayerState.h"
#include "TeamDeathmatchGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnded);
/**
 *
 */
UCLASS()
class TACTICALSHOOTER_API ATeamDeathmatchGameState : public AGameState
{
	GENERATED_BODY()

public:
	void AddPlayerState(APlayerState* PlayerState) override;

	virtual void IncreaseKillCount(ETeam Team, int32 ScorePerKill);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual int32 GetTeamAKillCount();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual int32 GetTeamBKillCount();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual int32 GetTeamAScore();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual int32 GetTeamBScore();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual ETeam GetWinner() { return Winner; };

	virtual void SetWinner(ETeam GameWinner);

	UPROPERTY(BlueprintAssignable)
		FOnMatchEnded OnMatchEnded;

protected:

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
		int32 TeamAKillCount = 0;
	UPROPERTY(Replicated)
		int32 TeamBKillCount = 0;

	UPROPERTY(Replicated)
		int32 TeamAScore = 0;
	UPROPERTY(Replicated)
		int32 TeamBScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Winner)
		ETeam Winner = ETeam::None;

	TArray<APlayerState*> TeamAPlayers;
	TArray<APlayerState*> TeamBPlayers;

	virtual void HandleMatchHasEnded() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
		virtual void OnRep_Winner();
};
