// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

/**
 * 
 */
class TACTICALSHOOTER_API CustomAnimationUtils
{
public:
	CustomAnimationUtils();
	~CustomAnimationUtils();

	static void PlayMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage);
};
