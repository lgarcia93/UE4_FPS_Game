// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TacticalShooter/Public/Interfaces/Throwable.h"
#include "Animation/AnimInstance.h"
#include "TacticalShooter/Public/Enums/Enums.h"
#include "Components/SkeletalMeshComponent.h"
#include "TacticalShooter/Public/Base/Equipment.h"
#include "ThrowableBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThrown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExpiry);

UCLASS()
class TACTICALSHOOTER_API AThrowableBase : public AEquipment, public IThrowable
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaTime) override;

	bool bWasThrown = false;

	FVector Velocity = FVector::ZeroVector;

	float TimeElapsedSinceThrown = 0;

	AActor* LastHitActor = nullptr;

	UPROPERTY(EditANywhere, BlueprintReadWrite)
		float ExpiryTime = 5;

	UPROPERTY(EditANywhere, BlueprintReadWrite)
		bool SafetyReleased = false;

	UPROPERTY(BlueprintAssignable)
		FOnThrown OnThrown;

	UPROPERTY(BlueprintAssignable)
		FOnExpiry OnExpiry;

	//Used for lethal damage or full effect
	UPROPERTY(EditANywhere, BlueprintReadWrite)
		float InnerRadius;

	//Used for atteanuating the effects of the throwable
	UPROPERTY(EditANywhere, BlueprintReadWrite)
		float OuterRadius;

	UPROPERTY(EditAnywhere, Category = "Throwable Info")
		float ThrowableScalarImpulse = 300;

	UPROPERTY(EditAnywhere, Category = "Animation Montage")
		UAnimMontage* FP_ThrowMontage;

	UPROPERTY(EditAnywhere, Category = "Animation Montage")
		UAnimMontage* TP_ThrowMontage;

	UPROPERTY(EditAnywhere, Category = "Arms Animation Montage")
		UAnimMontage* FP_CharacterThrowMontage;

	UPROPERTY(EditAnywhere, Category = "3P Animation Montage")
		UAnimMontage* TP_CharacterThrowMontage;


	UFUNCTION(BlueprintCallable)
		void PlayThrowAnimation();

	bool IsLocallyHeld();

	void SphereTrace();

#pragma region ThrowableInterface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Throw();

	virtual void Throw_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BeginThrow();

	virtual void BeginThrow_Implementation() override;
#pragma endregion

#pragma region Networking

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_BeginThrow();

	bool Server_BeginThrow_Validate();
	void Server_BeginThrow_Implementation();

	//Multicast
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multi_BeginThrow();

	bool Multi_BeginThrow_Validate();
	void Multi_BeginThrow_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Throw(FVector ThrowableLocation, FVector ForwardVector);

	bool Server_Throw_Validate(FVector ThrowableLocation, FVector ForwardVector);
	void Server_Throw_Implementation(FVector ThrowableLocation, FVector ForwardVector);

	//Multicast
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multi_Throw(FVector ThrowableLocation, FVector ForwardVector);

	bool Multi_Throw_Validate(FVector ThrowableLocation, FVector ForwardVector);
	void Multi_Throw_Implementation(FVector ThrowableLocation, FVector ForwardVector);
#pragma endregion
};
