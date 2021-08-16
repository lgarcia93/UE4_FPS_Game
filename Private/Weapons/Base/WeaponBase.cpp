#include "TacticalShooter/Public/Weapons/Base/WeaponBase.h"
#include "TacticalShooter/Public/Weapons/Base/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ScriptDelegates.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerState.h"
#include "TacticalShooter/Public/Character/PlayableCharacter.h"
#include "TacticalShooter/Public/PlayerState/TeamDeathmatchPlayerState.h"
#include "TacticalShooter/Public/Interfaces/GunHolder.h"
#include "TacticalShooter/Public/Animation/CustomAnimationUtils.h"
#include "Net/UnrealNetwork.h"


void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, CurrentMagazineAmmo);
	DOREPLIFETIME(AWeaponBase, bIsFiring);
	DOREPLIFETIME(AWeaponBase, CurrentFiringMode);
}

void AWeaponBase::PullTrigger_Implementation()
{
	if (!IsWeaponEmpty())
	{
		switch (GetCurrentFiringMode())
		{
		case EWeaponFiringMode::SingleShot:
			HandleSingleShot();
			break;
		case EWeaponFiringMode::FullAuto:
			HandleFullAutoFire();
			break;
		case EWeaponFiringMode::Burst3:
			HandleBurstFire();
			break;
		case EWeaponFiringMode::Burst2:
			HandleBurstFire();
			break;
		}
	}
	//TODO: Play empty gun sound
}

void AWeaponBase::ReleaseTrigger()
{
	SetIsFiring(false);

	OnStopShooting.Broadcast();

	if (!HasAuthority())
	{
		Server_Stop_Shooting();
	}
	else
	{
		Multi_Stop_Shooting();
	}

	GetWorld()->GetTimerManager().ClearTimer(FullAutoTimerHandle);
}

void AWeaponBase::HandleSingleShot()
{
	if (IsWeaponEmpty())
		return;

	PlayFireAnimation();

	CurrentMagazineAmmo -= 1;

	DoLineTraceAndSend();

	OnStartShooting.Broadcast();
}

void AWeaponBase::HandleFullAutoFire()
{
	//Fire first round
	PlayFireAnimation();

	DoLineTraceAndSend();

	OnStartShooting.Broadcast();

	SetIsFiring(true);

	int32 FireRate = 0;

	for (auto FiringModeInfo : GetFireRateInfoArray())
	{
		if (FiringModeInfo.FireMode == EWeaponFiringMode::FullAuto)
		{
			FireRate = FiringModeInfo.FireRate;
			break;
		}
	}

	float ShotsPerSecond = FireRate / 60;

	float ShotsInterval = 1.0f / ShotsPerSecond;

	GetWorldTimerManager().SetTimer(
		FullAutoTimerHandle,
		this,
		&AWeaponBase::FullAutoPeriodicFire,
		ShotsInterval,
		true,
		ShotsInterval
	);

	return;
}

void AWeaponBase::FullAutoPeriodicFire()
{
	if (IsWeaponEmpty())
	{
		ReleaseTrigger();

		return;
	}

	if (GetIsFiring()) {

		PlayFireAnimation();

		DoLineTraceAndSend();
	}

	CurrentMagazineAmmo -= 1;
}

void AWeaponBase::HandleBurstFire()
{

}

void AWeaponBase::Reload()
{
	if (CurrentMagazineAmmo < MagazineMaxAmmo) {

		PlayReloadAnimation();

		bool bIsWeaponEmpty = IsWeaponEmpty();

		//Adds one in the chamber if gun accepts.
		CurrentMagazineAmmo = MagazineMaxAmmo + (AcceptsOneInTheChamber && !bIsWeaponEmpty ? 1 : 0);

		if (!HasAuthority())
		{
			Server_Reload();
		}
		else
		{		
			Multi_Reload();
		}
	}
}

bool AWeaponBase::IsWeaponEmpty()
{
	return CurrentMagazineAmmo < 1;
}

TArray<FHitResult> AWeaponBase::PerformLineTrace(FVector MuzzleLocation, FRotator MuzzleRotation)
{
	FVector End = MuzzleLocation + MuzzleRotation.Vector() * 20000.0f;

	FCollisionQueryParams CollisionQueryParams;

	CollisionQueryParams.AddIgnoredActor(GetOwner());

	CollisionQueryParams.bReturnPhysicalMaterial = true;

	FCollisionObjectQueryParams CollisionObjectQueryParams;

	TArray<FHitResult> HitResults;

	FCollisionResponseParams CollisionResponse;

	GetWorld()->LineTraceMultiByChannel(OUT HitResults, MuzzleLocation, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionQueryParams, CollisionResponse);

	DrawDebugLine(GetWorld(), MuzzleLocation, End, FColor::Red, false, 5.0f, 0, 3.0f);

	return HitResults;
}

void AWeaponBase::PerformLineTraceAndProcessHits(FVector MuzzleLocation, FRotator MuzzleRotation)
{
	TArray<FHitResult> HitResults = PerformLineTrace(MuzzleLocation, MuzzleRotation);

	for (FHitResult& Hit : HitResults)
	{
		if (APlayableCharacter* PC = Cast<APlayableCharacter>(Hit.GetActor()))
		{
			PC->Hit(GetActorForwardVector(), Hit, Cast<APlayableCharacter>(GetOwner()));
		}
		else
		{
			OnBulletHit.Broadcast(Hit);
		}
	}
}

void AWeaponBase::CycleFireMode()
{
	if (GetCurrentFiringMode() == EWeaponFiringMode::FullAuto)
	{
		SetCurrentFiringMode(EWeaponFiringMode::SingleShot);
	}
	else
	{
		SetCurrentFiringMode(EWeaponFiringMode::FullAuto);
	}
}

void AWeaponBase::SetCurrentFiringMode(EWeaponFiringMode FiringMode)
{
	CurrentFiringMode = FiringMode;

	if (!HasAuthority())
		Server_Set_Fire_Mode(FiringMode);
}

void AWeaponBase::DoLineTraceAndSend()
{
	FVector Start = GetMuzzleLocation();
	FRotator Rot = GetFPMesh()->GetSocketRotation(FName("MuzzleSocket"));

	PerformLineTraceAndProcessHits(Start, Rot);

	if (!HasAuthority())
	{
		Server_Shoot(Start, Rot);
	}
	else
	{
		Multi_Shoot(Start, Rot);
	}
}

FVector AWeaponBase::GetMuzzleLocation()
{
	return GetFPMesh()->GetSocketLocation(FName("MuzzleSocket"));
}

bool AWeaponBase::IsGunLocallyHeld()
{
	if (GetOwner())
	{
		if (GetOwner()->Implements<UGunHolder>())
		{
			return IGunHolder::Execute_IsLocallyHeld(GetOwner());
		}
	}

	return false;
}

#pragma region PlayAnimations

void AWeaponBase::PlayAnimation(UAnimMontage* FirstPersonMontage,
						UAnimMontage* FirstPersonArmsMontage,
						UAnimMontage* ThirdPersonMontage,
						UAnimMontage* ThirdPersonArmsMontage
					)
{
	if (AActor* aOwner = GetOwner())
	{
		if (aOwner->Implements<UGunHolder>())
		{
			if (IGunHolder::Execute_IsLocallyHeld(aOwner))
			{
				//Play FP Animations

			//Arms
				CustomAnimationUtils::PlayMontage(IGunHolder::Execute_GetFPArmsMesh(aOwner), FirstPersonArmsMontage);

				//Gun
				CustomAnimationUtils::PlayMontage(FP_Mesh, FirstPersonMontage);
			}
			else
			{
				//Play TP Animations
				//Arms
				CustomAnimationUtils::PlayMontage(IGunHolder::Execute_GetTPMesh(aOwner), ThirdPersonArmsMontage);

				//Gun
				CustomAnimationUtils::PlayMontage(TP_Mesh, ThirdPersonMontage);
			}
		}
	}
}

void AWeaponBase::PlayReloadAnimation()
{
	PlayAnimation(
		GetReloadAnimationMontage(),
		GetArmsReloadAnimationMontage(),
		GetReloadAnimationMontage(),
		TP_MeshReloadEmptyAnimationMontage
	);
	
}
void AWeaponBase::PlayFireAnimation()
{
	PlayAnimation(
		GetFireAnimation(),
		GetArmsFireAnimation(),
		GetFireAnimation(),
		GetTPFireAnimation()
	);
}
#pragma endregion
#pragma region ServerShoot

void AWeaponBase::Server_Shoot_Implementation(FVector MuzzleLocation, FRotator MuzzleRotator)
{
	if (!IsGunLocallyHeld()) {

		CurrentMagazineAmmo -= 1;
		PerformLineTraceAndProcessHits(MuzzleLocation, MuzzleRotator);

		Multi_Shoot(MuzzleLocation, MuzzleRotator);
	}

}
#pragma endregion
#pragma region ServerStartShooting

void AWeaponBase::Server_Start_Shooting_Implementation()
{
	OnStartShooting.Broadcast();

	Multi_Start_Shooting();
}
#pragma endregion
#pragma region MultiStartShooting
void AWeaponBase::Multi_Start_Shooting_Implementation()
{
	if (!IsGunLocallyHeld())
	{
		OnStartShooting.Broadcast();
	}
}
#pragma endregion
#pragma region ServerStopShooting
void AWeaponBase::Server_Stop_Shooting_Implementation()
{
	OnStopShooting.Broadcast();

	Multi_Stop_Shooting();
}
#pragma endregion
#pragma region MulticastStopShooting

void AWeaponBase::Multi_Stop_Shooting_Implementation()
{
	if (!IsGunLocallyHeld())
		OnStopShooting.Broadcast();
}
#pragma endregion
#pragma region MulticastShoot

void AWeaponBase::Multi_Shoot_Implementation(FVector MuzzleLocation, FRotator MuzzleRotator)
{
	if (!IsGunLocallyHeld())
	{
		PlayFireAnimation();

		PerformLineTraceAndProcessHits(MuzzleLocation, MuzzleRotator);
	}
}
#pragma endregion
#pragma region ServerReload

void AWeaponBase::Server_Reload_Implementation()
{
	PlayReloadAnimation();

	bool bIsWeaponEmpty = IsWeaponEmpty();

	//Adds one in the chamber if gun accepts.
	CurrentMagazineAmmo = MagazineMaxAmmo + (AcceptsOneInTheChamber && !bIsWeaponEmpty ? 1 : 0);

	Multi_Reload();
}
#pragma endregion
#pragma region MulticastReload

void AWeaponBase::Multi_Reload_Implementation()
{
	PlayReloadAnimation();
}

void AWeaponBase::Server_Set_Fire_Mode_Implementation(EWeaponFiringMode NewFiringMode)
{
	SetCurrentFiringMode(NewFiringMode);	
}
#pragma endregion

UAnimMontage* AWeaponBase::GetFireAnimation()
{
	if (CurrentMagazineAmmo - 1 <= 0)
	{
		if (FireLastShotAnimationMontage)
		{
			return FireLastShotAnimationMontage;
		}
	}

	return FireAnimationMontage;
}

UAnimMontage* AWeaponBase::GetArmsFireAnimation()
{
	if (CurrentMagazineAmmo - 1 <= 0)
	{
		if (ArmsFireLastShotAnimationMontage)
		{
			return ArmsFireLastShotAnimationMontage;
		}
	}

	return ArmsFireAnimationMontage;
}

UAnimMontage* AWeaponBase::GetTPFireAnimation()
{
	if (CurrentMagazineAmmo - 1 <= 0)
	{
		if (TP_MeshFireLastShotAnimationMontage)
		{
			return TP_MeshFireLastShotAnimationMontage;
		}
	}

	return TP_MeshFireAnimationMontage;
}
