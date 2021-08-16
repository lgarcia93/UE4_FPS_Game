// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "TacticalShooter/Public/Weapons/Base/WeaponBase.h"
#include "TacticalShooter/Public/Interfaces/GunHolder.h"
#include "TacticalShooter/Public/Enums/Enums.h"
#include "TacticalShooter/Public/Base/Equipment.h"
#include "TacticalShooter/Public/Weapons/Base/ThrowableBase.h"
#include "PlayableCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTakeHit, FVector, BulletForwardVector, FVector, HitLocation, FName, BoneName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletFlyBy, FVector, Location);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, int32, DelayBeforeRespawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLookAtEquipable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLookAwayFromEquipable);
/**
 *
 */

UCLASS()
class TACTICALSHOOTER_API APlayableCharacter : public ABaseCharacter
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeaponBase> CurrentWeaponClass;

public:
	void BeginPlay() override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void SetIsReloading(bool IsReloading) { bIsReloading = IsReloading; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsReloading() { return bIsReloading; };

	void HitByExplosion(APlayableCharacter* DamageCauser);
	void Hit(FVector BulletForwardVector, FHitResult HitResult, APlayableCharacter* DamageCauser);
	void Kill(APlayableCharacter* DamageCauser);
protected:
	//Replicated variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated/*Using = OnRep_CurrentEquipable*/)
		AEquipment* CurrentEquipable;

	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsReloading;

	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsDead = false;

	FTimerHandle AfterDeathSpawnTimerHandle;
	FTimerHandle EquipableLineTraceTimer;

	UPROPERTY(EditAnywhere, Category = "Character Equipment", Replicated)
		AWeaponBase* PrimaryWeapon;

	UPROPERTY(EditAnywhere, Category = "Character Equipment", Replicated)
		AWeaponBase* SecondaryWeapon;

	UPROPERTY(EditAnywhere, Category = "Character Equipment", Replicated)
		TArray<AThrowableBase*> Throwables;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AEquipment* NextEquipableToSwitchTo = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanSwitchEquipable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsTransitioningEquipable = false;


	UPROPERTY(BlueprintAssignable)
		FTakeHit OnTakeHit;

	UPROPERTY(BlueprintAssignable)
		FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
		FOnLookAtEquipable OnLookAtEquipable;

	UPROPERTY(BlueprintAssignable)
		FOnLookAwayFromEquipable OnLookAwayFromEquipable;

	UPROPERTY(BlueprintAssignable)
		FOnBulletFlyBy OnBulletFlyBy;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		EEquipable GetCurrentEquipableEnum();

	AEquipment* CurrentLookAtEquipable = nullptr;

	UFUNCTION(BlueprintCallable)
		void OnUnequipAnimationEnded();

	UFUNCTION(BlueprintCallable)
		void OnEquipAnimationEnded();

	virtual void OnFire();
	virtual void OnStopFire();
	virtual void OnFireModeSelect();
	virtual void OnReload();
	virtual void OnPrimaryWeapon();
	virtual void OnSecondaryWeapon();
	virtual void OnCycleThrowable();
	virtual void Respawn();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsDead() { return bIsDead; };

	void PeriodicEquipableLinetrace();

	void StartPeriodicEquipableLineTrace();
	void StopPeriodicEquipableLineTrace();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		inline AEquipment* GetCurrentLookAtEquipable() { return CurrentLookAtEquipable; };

	UFUNCTION(BlueprintCallable)
		void EquipCurrentLookAtEquipable();

	UFUNCTION(BlueprintCallable)
		void WantToSwitchEquipable(AEquipment* DesiredEquipable);

	//Multicast Functions

	UFUNCTION(NetMulticast, Reliable)
		void Multi_OnHit(FVector BulletForwardVector, FHitResult HitResult);

	void Multi_OnHit_Implementation(FVector BulletForwardVector, FHitResult HitResult);

	UFUNCTION(NetMulticast, Reliable)
		void Multi_OnHitByExplosion();

	void Multi_OnHitByExplosion_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void Multi_OnDeath(int32 TimeBeforeSpawn);

	void Multi_OnDeath_Implementation(int32 TimeBeforeSpawn);

	UFUNCTION(Server, Reliable)
		void Server_Equip(AEquipment* Equipable);

	void Server_Equip_Implementation(AEquipment* Equipable);

	UFUNCTION(NetMulticast, Reliable)
		void Multi_Equip(AEquipment* Equipable);

	void Multi_Equip_Implementation(AEquipment* Equipable);

	UFUNCTION(Server, Reliable)
		void Server_UnEquip_Equipable(AEquipment* Equipable);

	void Server_UnEquip_Equipable_Implementation(AEquipment* Equipable);

	UFUNCTION(NetMulticast, Reliable)
		void Multi_UnEquip_Equipable(AEquipment* Equipable);

	void Multi_UnEquip_Equipable_Implementation(AEquipment* Equipable);

	UFUNCTION(Server, Reliable)
		void Server_Drop_Equipable(AEquipment* Equipable);

	void Server_Drop_Equipable_Implementation(AEquipment* Equipable);

	UFUNCTION(NetMulticast, Reliable)
		void Multi_Drop_Equipable(AEquipment* Equipable);

	void Multi_Drop_Equipable_Implementation(AEquipment* Equipable);


	UFUNCTION(Server, Reliable)
		void Server_Drop_All_Equipables();

	void Server_Drop_All_Equipables_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void Multi_Drop_All_Equipables();

	void Multi_Drop_All_Equipables_Implementation();

	virtual bool IsReloading() override;
	virtual bool IsDead() override;
	virtual bool IsSprinting() override;

	UFUNCTION(BlueprintCallable)
		void DropAllEquipables();

	UFUNCTION(BlueprintCallable)
		void DropCurrentEquipable();

	UFUNCTION(BlueprintCallable)
		void EquipEquipable(AEquipment* NewEquipable);

	UFUNCTION(BlueprintCallable)
		void UnEquipEquipable(AEquipment* EquipableToUnEquip);

	void DropAllEquipables_Functionality();

	AWeaponBase* CastCurrentEquipableToWeapon();
	AThrowableBase* CastCurrentEquipableToThrowable();
};