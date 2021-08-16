// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SkeletalMeshComponent.h"
#include "GunHolder.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGunHolder : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class TACTICALSHOOTER_API IGunHolder
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		USkeletalMeshComponent* GetFPArmsMesh();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		USkeletalMeshComponent* GetTPMesh();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		FRotator GetHolderControlRotation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsLocallyHeld();


	virtual bool IsDead() { return false; };
	virtual bool IsReloading() { return false; };
	virtual bool IsSprinting() { return false; };	
};

