// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/Animation/CustomAnimationUtils.h"

CustomAnimationUtils::CustomAnimationUtils()
{
}

CustomAnimationUtils::~CustomAnimationUtils()
{
}

void CustomAnimationUtils::PlayMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage)
{
	if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(Montage);
	}
}