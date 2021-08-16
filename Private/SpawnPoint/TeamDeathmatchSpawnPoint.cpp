// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/SpawnPoint/TeamDeathmatchSpawnPoint.h"

ATeamDeathmatchSpawnPoint::ATeamDeathmatchSpawnPoint() {
	isUsed = false;
}

void ATeamDeathmatchSpawnPoint::SetIsUsed()
{
	isUsed = true;
}

bool ATeamDeathmatchSpawnPoint::GetIsUsed()
{
	return isUsed;
}