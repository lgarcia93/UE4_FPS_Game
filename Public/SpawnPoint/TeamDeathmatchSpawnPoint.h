// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "TeamDeathmatchSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class TACTICALSHOOTER_API ATeamDeathmatchSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	ATeamDeathmatchSpawnPoint();

	void SetIsUsed();
	bool GetIsUsed();

protected:
	bool isUsed;
};
