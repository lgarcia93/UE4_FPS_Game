// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/Character/PlayableCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "TacticalShooter/Public/PlayerState/TeamDeathmatchPlayerState.h"
#include "TacticalShooter/Public/GameMode/TeamDeathmatchGameMode.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "TacticalShooter/Public/Animation/CustomAnimationUtils.h"

// called to bind functionality to input
void APlayableCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayableCharacter::OnFire);

	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayableCharacter::OnStopFire);
	PlayerInputComponent->BindAction("FireModeSelect", IE_Released, this, &APlayableCharacter::OnFireModeSelect);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayableCharacter::OnReload);
	PlayerInputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &APlayableCharacter::OnPrimaryWeapon);
	PlayerInputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &APlayableCharacter::OnSecondaryWeapon);
	PlayerInputComponent->BindAction("CycleThrowable", IE_Pressed, this, &APlayableCharacter::OnCycleThrowable);
}

void APlayableCharacter::GetLifetimeReplicatedProps
(
	TArray < FLifetimeProperty >& OutLifetimeProps
) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayableCharacter, CurrentEquipable);
	DOREPLIFETIME(APlayableCharacter, bIsReloading);
	DOREPLIFETIME(APlayableCharacter, bIsADS);
	DOREPLIFETIME(APlayableCharacter, bIsSprinting);
	DOREPLIFETIME(APlayableCharacter, bIsDead);
	DOREPLIFETIME(APlayableCharacter, PrimaryWeapon);
	DOREPLIFETIME(APlayableCharacter, SecondaryWeapon);
	DOREPLIFETIME(APlayableCharacter, Throwables);
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FActorSpawnParameters ActorSpawnParameters;

		ActorSpawnParameters.Owner = this;

		AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(CurrentWeaponClass, ActorSpawnParameters);

		if (NewWeapon)
		{
			PrimaryWeapon = NewWeapon;

			EquipEquipable(NewWeapon);

			CurrentEquipable = NewWeapon;
		}
	}

	if (IsLocallyControlled())
	{
		StartPeriodicEquipableLineTrace();
	}
}

void APlayableCharacter::StartPeriodicEquipableLineTrace()
{
	GetWorldTimerManager().SetTimer(
		EquipableLineTraceTimer,
		this,
		&APlayableCharacter::PeriodicEquipableLinetrace,
		0.2,
		true,
		3
	);
}

void APlayableCharacter::StopPeriodicEquipableLineTrace()
{
	GetWorld()->GetTimerManager().ClearTimer(EquipableLineTraceTimer);
}

void APlayableCharacter::EquipCurrentLookAtEquipable()
{
	if (IsLocallyControlled())
	{
		if (CurrentLookAtEquipable)
		{
			if (!CurrentLookAtEquipable->IsCurrentlyHeld())
			{
				//If current equipable and lookAtEquipable both are guns, drop the current
				if (CurrentEquipable->Implements<UGun>() && CurrentLookAtEquipable->Implements<UGun>())
				{
					DropCurrentEquipable();
				}

				EquipEquipable(CurrentLookAtEquipable);
			}
		}
	}
}

/// <summary>
/// Will first Unequip the current equipable if valid,
/// then in the animation blueprint, the function OnUnequipAnimationEnded must (and will be called)
/// to continue with the Equip animation and stuff of the DesiredEquipable
/// </summary>
/// <param name="DesiredEquipable"></param>
void APlayableCharacter::WantToSwitchEquipable(AEquipment* DesiredEquipable)
{
	if (CurrentEquipable == nullptr)
	{
		EquipEquipable(DesiredEquipable);

		return;
	}

	if (bCanSwitchEquipable && CurrentEquipable)
	{
		NextEquipableToSwitchTo = DesiredEquipable;

		UnEquipEquipable(CurrentEquipable);
	}
}

void APlayableCharacter::PeriodicEquipableLinetrace()
{
	FVector CameraLocation = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector CameraForwardVector = GetFirstPersonCameraComponent()->GetForwardVector();

	FVector End = CameraLocation + CameraForwardVector * 200;

	FCollisionQueryParams CollisionQueryParams;

	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(CurrentEquipable);

	CollisionQueryParams.bReturnPhysicalMaterial = false;

	FCollisionObjectQueryParams CollisionObjectQueryParams;

	FHitResult HitResult;

	FCollisionResponseParams CollisionResponse;

	GetWorld()->LineTraceSingleByChannel(OUT HitResult, CameraLocation, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams, CollisionResponse);

	if (AActor* Actor = HitResult.GetActor())
	{
		if (AEquipment* Equipment = Cast<AEquipment>(Actor))
		{
			if (!Equipment->IsCurrentlyHeld())
			{
				if (Equipment != CurrentLookAtEquipable)
				{
					CurrentLookAtEquipable = Equipment;

					OnLookAtEquipable.Broadcast();
				}
			}

			return;
		}
	}

	CurrentLookAtEquipable = nullptr;

	OnLookAwayFromEquipable.Broadcast();
}

EEquipable APlayableCharacter::GetCurrentEquipableEnum()
{
	if (CurrentEquipable)
	{
		return CurrentEquipable->GetEnumIdentifier();
	}

	return EEquipable::Unnarmed;
}

void APlayableCharacter::OnUnequipAnimationEnded()
{

	if (CurrentEquipable)
	{
		CurrentEquipable->SetHidden(true);
	}

	EquipEquipable(NextEquipableToSwitchTo);
}

void APlayableCharacter::OnEquipAnimationEnded()
{

}

void APlayableCharacter::OnFire()
{
	if (bIsReloading || bIsTransitioningEquipable || bIsSprinting)
		return;

	if (CurrentEquipable->Implements<UGun>())
	{
		AWeaponBase* CurrentWeapon = Cast<AWeaponBase>(CurrentEquipable);

		if (CurrentWeapon)
		{
			CurrentWeapon->PullTrigger();
		}
	}

	if (CurrentEquipable->Implements<UThrowable>())
	{
		IThrowable::Execute_BeginThrow(CurrentEquipable);
	}
}

void APlayableCharacter::OnStopFire()
{
	if (AWeaponBase* Weapon = CastCurrentEquipableToWeapon())
	{
		Weapon->ReleaseTrigger();
	}
}

void APlayableCharacter::OnFireModeSelect()
{
	if (bIsSprinting || bIsReloading)
		return;

	if (AWeaponBase* Weapon = CastCurrentEquipableToWeapon())
	{
		Weapon->CycleFireMode();
	}
}

void APlayableCharacter::OnReload()
{
	if (bIsSprinting || bIsReloading)
		return;

	if (AWeaponBase* Weapon = CastCurrentEquipableToWeapon())
	{
		Weapon->Reload();
	}
}

void APlayableCharacter::OnPrimaryWeapon()
{
	if (bIsSprinting || bIsReloading)
		return;

	if (PrimaryWeapon && (CurrentEquipable != PrimaryWeapon) && !bIsReloading)
	{
		WantToSwitchEquipable(PrimaryWeapon);
	}
}

void APlayableCharacter::OnSecondaryWeapon()
{
}

void APlayableCharacter::OnCycleThrowable()
{
	if (Throwables.Num())
	{
		if (AThrowableBase* CurrentThrowable = Cast<AThrowableBase>(CurrentEquipable))
		{
			if (Throwables.Num() > 1)
			{
				int32 Index;

				Throwables.Find(CurrentThrowable, Index);

				Index++;

				if (Index >= Throwables.Num())
				{
					Index = 0;
				}

				NextEquipableToSwitchTo = Throwables[Index];

				UnEquipEquipable(CurrentEquipable);
			}
		}
		else
		{
			NextEquipableToSwitchTo = Throwables[0];

			UnEquipEquipable(CurrentEquipable);
		}
	}
}

void APlayableCharacter::HitByExplosion(APlayableCharacter* DamageCauser)
{
	if (bIsDead)
		return;

	if (HasAuthority())
	{
		Multi_OnHitByExplosion();
	}

	//TODO check if player should be killed
	if (true)
	{
		Kill(DamageCauser);
	}
}

void APlayableCharacter::Hit(FVector BulletForwardVector, FHitResult HitResult, APlayableCharacter* DamageCauser)
{
	if (bIsDead)
		return;

	if (HasAuthority())
	{
		Multi_OnHit(BulletForwardVector, HitResult);
	}

	//TODO check if player should be killed
	if (true)
	{
		Kill(DamageCauser);
	}
}

void APlayableCharacter::Kill(APlayableCharacter* DamageCauser)
{
	if (HasAuthority() && !bIsDead)
	{
		DropAllEquipables();

		bIsDead = true;

		if (DamageCauser) {
			if (ATeamDeathmatchPlayerState* DamageCauserPS = Cast<ATeamDeathmatchPlayerState>(DamageCauser->GetPlayerState()))
			{
				if (ATeamDeathmatchGameMode* GM = Cast<ATeamDeathmatchGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					GM->AddKill(DamageCauserPS);
				}
			}

			if (ATeamDeathmatchPlayerState* PS = Cast<ATeamDeathmatchPlayerState>(GetPlayerState()))
			{
				PS->SetIsDead(true);

				int32 DelayBeforeSpawn = GetWorld()->GetAuthGameMode<ATeamDeathmatchGameMode>()->GetSecondsBetweenDeathAndSpawn();

				GetWorldTimerManager().SetTimer(AfterDeathSpawnTimerHandle, this, &APlayableCharacter::Respawn, DelayBeforeSpawn, false);

				Multi_OnDeath(DelayBeforeSpawn);
			}
		}
	}
}

void APlayableCharacter::Respawn()
{
	if (HasAuthority())
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			GetWorld()->GetAuthGameMode<ATeamDeathmatchGameMode>()->SpawnPlayer(PC);

			Destroy(true);
		}
	}
}

void APlayableCharacter::Multi_OnHit_Implementation(FVector BulletForwardVector, FHitResult HitResult)
{
	OnTakeHit.Broadcast(BulletForwardVector, HitResult.Location, HitResult.BoneName);
}

void APlayableCharacter::Multi_OnHitByExplosion_Implementation()
{
	OnTakeHit.Broadcast(FVector::ZeroVector, FVector::ZeroVector, FName("head"));
}

void APlayableCharacter::Multi_OnDeath_Implementation(int32 TimeBeforeSpawn)
{
	if (IsLocallyControlled())
	{
		OnDeath.Broadcast(TimeBeforeSpawn);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (CurrentEquipable)
	{
		CurrentEquipable->Drop();
	}

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
}

void APlayableCharacter::Server_Equip_Implementation(AEquipment* Equipable)
{
	CurrentEquipable = Equipable;

	Equipable->SetOwner(this);

	Equipable->Equip();

	if (AThrowableBase* NewThrowable = Cast<AThrowableBase>(Equipable))
	{
		Throwables.Add(NewThrowable);
	}

	if (AWeaponBase* NewWeapon = Cast<AWeaponBase>(Equipable))
	{
		PrimaryWeapon = NewWeapon;
	}

	Multi_Equip(Equipable);
}

void APlayableCharacter::Multi_Equip_Implementation(AEquipment* Equipable)
{
	if (!IsLocallyControlled())
	{
		CurrentEquipable->Equip();
	}
}

bool APlayableCharacter::IsReloading()
{
	return bIsReloading;
}

bool APlayableCharacter::IsDead()
{
	return bIsDead;
}

bool APlayableCharacter::IsSprinting()
{
	return bIsSprinting;
}

void APlayableCharacter::DropAllEquipables()
{
	DropAllEquipables_Functionality();

	if (HasAuthority())
	{
		Server_Drop_All_Equipables();
	}
	else
	{
		Multi_Drop_All_Equipables();
	}

}

void APlayableCharacter::DropCurrentEquipable()
{
	CurrentEquipable->Drop();

	if (HasAuthority())
	{
		Multi_Drop_Equipable(CurrentEquipable);
	}
	else
	{
		Server_Drop_Equipable(CurrentEquipable);
	}
}

void APlayableCharacter::EquipEquipable(AEquipment* NewEquipable)
{
	if (NewEquipable)
	{
		NewEquipable->SetOwner(this);

		if (CurrentEquipable)
			CurrentEquipable->SetHidden(false);

		NewEquipable->Equip();

		CurrentEquipable = NewEquipable;

		if (AThrowableBase* NewThrowable = Cast<AThrowableBase>(NewEquipable))
		{
			Throwables.Add(NewThrowable);
		}

		if (AWeaponBase* NewWeapon = Cast<AWeaponBase>(NewEquipable))
		{
			PrimaryWeapon = NewWeapon;
		}

		if (!HasAuthority())
		{
			Server_Equip(NewEquipable);
		}
	}
}

void APlayableCharacter::UnEquipEquipable(AEquipment* EquipableToUnEquip)
{
	if (EquipableToUnEquip)
	{
		EquipableToUnEquip->Unequip(true);

		if (HasAuthority())
		{
			Multi_UnEquip_Equipable(EquipableToUnEquip);
		}
		else
		{
			Server_UnEquip_Equipable(EquipableToUnEquip);
		}
	}
}

void APlayableCharacter::DropAllEquipables_Functionality()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->Drop();
	}

	for (auto ThrowableItem : Throwables)
	{
		ThrowableItem->Drop();
	}
}

AWeaponBase* APlayableCharacter::CastCurrentEquipableToWeapon()
{
	return Cast<AWeaponBase>(CurrentEquipable);
}

AThrowableBase* APlayableCharacter::CastCurrentEquipableToThrowable()
{
	return Cast<AThrowableBase>(CurrentEquipable);
}

#pragma region UnequipEquipableServerAndMulticast
void APlayableCharacter::Server_UnEquip_Equipable_Implementation(AEquipment* Equipable)
{
	Equipable->Unequip(true);

	Multi_UnEquip_Equipable(Equipable);
}

void APlayableCharacter::Multi_UnEquip_Equipable_Implementation(AEquipment* Equipable)
{
	if (!IsLocallyControlled())
	{
		if (Equipable)
		{
			Equipable->Unequip(true);
		}
	}
}
#pragma endregion

#pragma region DropEquipableServerAndMulticast
void APlayableCharacter::Server_Drop_Equipable_Implementation(AEquipment* Equipable)
{
	if (!IsLocallyControlled())
	{
		if (Equipable)
		{

			Equipable->Drop();

			Multi_Drop_Equipable(Equipable);

			if (PrimaryWeapon == Equipable)
				PrimaryWeapon = nullptr;

			if (CurrentEquipable == Equipable)
				CurrentEquipable = nullptr;
		}
	}
}

void APlayableCharacter::Multi_Drop_Equipable_Implementation(AEquipment* Equipable)
{
	if (!IsLocallyControlled())
	{
		if (Equipable)
		{
			Equipable->Drop();
		}
	}
}
#pragma endregion

#pragma region DropAllEquipablesServerAndMulticast
void APlayableCharacter::Server_Drop_All_Equipables_Implementation()
{
	if (!IsLocallyControlled())
	{
		DropAllEquipables_Functionality();

		Multi_Drop_All_Equipables();
	}
}

void APlayableCharacter::Multi_Drop_All_Equipables_Implementation()
{
	if (!IsLocallyControlled())
	{
		DropAllEquipables_Functionality();
	}
}
#pragma endregion