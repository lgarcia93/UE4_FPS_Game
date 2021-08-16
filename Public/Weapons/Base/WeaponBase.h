// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ScriptDelegates.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "TacticalShooter/Public/Enums/Enums.h"
#include "TacticalShooter/Public/Interfaces/Gun.h"
#include "TacticalShooter/Public/Base/Equipment.h"
#include "WeaponBase.generated.h"


USTRUCT(BlueprintType)
struct FFireRateInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EWeaponFiringMode FireMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 FireRate;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBulletHit, FHitResult, HitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartShooting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopShooting);

UCLASS()
class TACTICALSHOOTER_API AWeaponBase : public AEquipment, public IGun
{
	GENERATED_BODY()

public:
	AWeaponBase() {
		CurrentMagazineAmmo = MagazineMaxAmmo;
	};
protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, Replicated)
		int CurrentMagazineAmmo = 0;

	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsFiring = false;

	FTimerHandle FullAutoTimerHandle;


	UPROPERTY(Replicated)
		EWeaponFiringMode CurrentFiringMode = EWeaponFiringMode::SingleShot;
#pragma region Properties

	UPROPERTY(EditAnywhere, Category = "WeaponInfo")
		EWeaponType WeaponType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Info")
		int MagazineMaxAmmo;

	UPROPERTY(EditAnywhere, Category = "Weapon Info")
		bool AcceptsOneInTheChamber;

	UPROPERTY(EditAnywhere, Category = "Weapon Info")
		TArray<EWeaponFiringMode> FiringModes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
		TArray<FFireRateInfo> FireRateInfoArray;

	//Fire
	UPROPERTY(EditAnywhere, Category = "1P Arms Animations")
		UAnimMontage* ArmsFireAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "1P Gun Animations")
		UAnimMontage* FireAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "1P Arms Animations")
		UAnimMontage* ArmsFireLastShotAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "1P Gun Animations")
		UAnimMontage* FireLastShotAnimationMontage;

	//Reload
	UPROPERTY(EditAnywhere, Category = "1P Arms Animations")
		UAnimMontage* ArmsReloadAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "1P Gun Animations")
		UAnimMontage* ReloadAnimationMontage;

	//Reload Empty
	UPROPERTY(EditAnywhere, Category = "1P Arms Animations")
		UAnimMontage* ArmsReloadEmptyAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "1P Gun Animations")
		UAnimMontage* ReloadEmptyAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "1P Gun Animations")
		UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, Category = "1P Arms Animations")
		UAnimMontage* Arms_EquipMontage;

	UPROPERTY(EditAnywhere, Category = "1P Gun Animations")
		UAnimMontage* UnEquipMontage;

	UPROPERTY(EditAnywhere, Category = "1P Arms Animations")
		UAnimMontage* Arms_UnEquipMontage;

	UPROPERTY(EditAnywhere, Category = "3P Arms Animations")
		UAnimMontage* TP_MeshFireAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "3P Arms Animations")
		UAnimMontage* TP_MeshFireLastShotAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "3P Arms Animations")
		UAnimMontage* TP_MeshReloadAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "3P Arms Animations")
		UAnimMontage* TP_MeshReloadEmptyAnimationMontage;

	UPROPERTY(EditAnywhere, Category = "3P Arms Animations")
		UAnimMontage* TP_MeshEquipMontage;

	UPROPERTY(EditAnywhere, Category = "3P Arms Animations")
		UAnimMontage* TP_MeshUnEquipMontage;
#pragma endregion

	UPROPERTY(BlueprintAssignable)
		FOnStartShooting OnStartShooting;

	UPROPERTY(BlueprintAssignable)
		FOnStopShooting OnStopShooting;

	UPROPERTY(BlueprintAssignable)
		FBulletHit OnBulletHit;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		EWeaponType GetWeaponType() { return WeaponType; };

	UAnimMontage* GetFireAnimation();
	UAnimMontage* GetArmsFireAnimation();
	UAnimMontage* GetTPFireAnimation();


	void PlayFireAnimation();
	void PlayReloadAnimation();
	void PlayAnimation(UAnimMontage* FirstPersonMontage,
						UAnimMontage* FirstPersonArmsMontage,
						UAnimMontage* ThirdPersonMontage,
						UAnimMontage* ThirdPersonArmsMontage
					);

	UFUNCTION(Server, Reliable)
		void Server_Shoot(FVector MuzzleLocation, FRotator MuzzleRotator);
		
	void Server_Shoot_Implementation(FVector MuzzleLocation, FRotator MuzzleRotator);

	UFUNCTION(Server, Reliable)
		void Server_Start_Shooting();

	void Server_Start_Shooting_Implementation();

	//Multicast
	UFUNCTION(NetMulticast, Reliable)
		void Multi_Start_Shooting();

	void Multi_Start_Shooting_Implementation();

	UFUNCTION(Server, Reliable)
		void Server_Stop_Shooting();

	void Server_Stop_Shooting_Implementation();

	//Multicast
	UFUNCTION(NetMulticast, Reliable)
		void Multi_Stop_Shooting();

	void Multi_Stop_Shooting_Implementation();

	//Multicast
	UFUNCTION(NetMulticast, Reliable)
		void Multi_Shoot(FVector MuzzleLocation, FRotator MuzzleRotator);

	void Multi_Shoot_Implementation(FVector MuzzleLocation, FRotator MuzzleRotator);


	UFUNCTION(Server, Reliable)
		void Server_Reload();

	void Server_Reload_Implementation();

	//Multicast
	UFUNCTION(NetMulticast, Reliable)
		void Multi_Reload();

	void Multi_Reload_Implementation();

	UFUNCTION(Server, Reliable)
		void Server_Set_Fire_Mode(EWeaponFiringMode NewFiringMode);

	void Server_Set_Fire_Mode_Implementation(EWeaponFiringMode NewFiringMode);


	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsGunLocallyHeld();

public:
	UFUNCTION()
		void ReleaseTrigger();

	UFUNCTION()
		void HandleSingleShot();

	UFUNCTION()
		void HandleFullAutoFire();

	UFUNCTION()
		void FullAutoPeriodicFire();

	UFUNCTION()
		void HandleBurstFire();

	UFUNCTION()
		void Reload();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		inline TArray<EWeaponFiringMode> GetWeaponFiringModes() { return FiringModes; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
		inline EWeaponFiringMode GetCurrentFiringMode() { return CurrentFiringMode; };

	UFUNCTION(BlueprintCallable)
		void SetCurrentFiringMode(EWeaponFiringMode FiringMode);

	UFUNCTION(BlueprintCallable)
		TArray<FFireRateInfo> GetFireRateInfoArray() { return FireRateInfoArray; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsWeaponEmpty();

	UFUNCTION(BlueprintCallable)
		bool GetIsFiring() { return bIsFiring; };
	void SetIsFiring(bool IsFiring) { bIsFiring = IsFiring; };
		
	inline UAnimMontage* GetTPReloadAnimationMontage() { return TP_MeshReloadAnimationMontage; };
	inline UAnimMontage* GetArmsFireAnimationMontage() { return ArmsFireAnimationMontage; };
	inline UAnimMontage* GetArmsReloadAnimationMontage() {
		return IsWeaponEmpty() ? ArmsReloadEmptyAnimationMontage : ArmsReloadAnimationMontage;
	};

	inline UAnimMontage* GetReloadAnimationMontage() {
		return IsWeaponEmpty() ? ReloadEmptyAnimationMontage : ReloadAnimationMontage;
	};

	TArray<FHitResult> PerformLineTrace(FVector MuzzleLocation, FRotator MuzzleRotation);

	void PerformLineTraceAndProcessHits(FVector MuzzleLocation, FRotator MuzzleRotation);

#pragma region IGun

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void PullTrigger();

	virtual void PullTrigger_Implementation() override;

#pragma endregion
	virtual void CycleFireMode();

	virtual void DoLineTraceAndSend();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual FVector GetMuzzleLocation();
};
