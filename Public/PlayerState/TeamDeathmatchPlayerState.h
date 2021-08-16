// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TacticalShooter/Public/Enums/Enums.h"
#include "TeamDeathmatchPlayerState.generated.h"

/**
 *
 */
UCLASS()
class TACTICALSHOOTER_API ATeamDeathmatchPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

	UPROPERTY(Replicated)
		bool bIsDead = false;

	UPROPERTY(Replicated)
		uint32 PlayerCurrentScore = 0;

	UPROPERTY(Replicated)
		uint32 PlayerKillCount = 0;

	UPROPERTY(Replicated)
		ETeam Team = ETeam::None;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual bool GetIsDead();

	virtual void SetIsDead(bool IsDead);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual int32 GetCurrentScore();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual int32 GetPlayerKillCount();

	virtual void IncreasePlayerKillCount();

	virtual void IncreaseScore(int32 Points);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual ETeam GetTeam();

	virtual void SetTeam(ETeam NewTeam);



protected:
	virtual void PostInitializeComponents() override;
};
