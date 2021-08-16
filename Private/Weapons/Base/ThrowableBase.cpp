// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/Weapons/Base/ThrowableBase.h"
#include "TacticalShooter/Public/Interfaces/GunHolder.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TacticalShooter/Public/Animation/CustomAnimationUtils.h"
#include "TacticalShooter/Public/Character/PlayableCharacter.h"
#include "Net/UnrealNetwork.h"

// Called when the game starts or when spawned

void AThrowableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TimeElapsedSinceThrown < ExpiryTime)
	{
		FVector LineStart = GetActorLocation();
		FVector LineEnd = (Velocity * DeltaTime) + GetActorLocation();

		if (bWasThrown)
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;

				CollisionParams.AddIgnoredActor(this);

				if (LastHitActor)
				{
					CollisionParams.AddIgnoredActor(LastHitActor);
				}

				if (World->LineTraceSingleByChannel(HitResult, LineStart, LineEnd, ECollisionChannel::ECC_Visibility, CollisionParams))
				{
					if (AActor* HitActor = HitResult.GetActor())
					{
						LastHitActor = HitActor;

						Velocity = 0.3 * (-2 * FVector::DotProduct(Velocity, HitResult.Normal) * HitResult.Normal + Velocity);

						SetActorLocation(HitResult.Location);
					}
				}
				else
				{
					LastHitActor = nullptr;
					SetActorLocation(LineEnd);

					Velocity = Velocity + FVector(0, 0, -50);
				}
							
				DrawDebugLine(World, LineStart, LineEnd, FColor::Orange, true);
			}
		}


		if (bWasThrown || SafetyReleased)
		{
			TimeElapsedSinceThrown += DeltaTime;
		}
		
		if (TimeElapsedSinceThrown >= ExpiryTime)
		{
			OnExpiry.Broadcast();

			if (HasAuthority())
			{
				SphereTrace();
			}

			PrimaryActorTick.bCanEverTick = false;
		}

	}	
}

void AThrowableBase::PlayThrowAnimation()
{
	if (GetOwner())
	{
		if (GetOwner()->Implements<UGunHolder>())
		{
			CustomAnimationUtils::PlayMontage(IGunHolder::Execute_GetFPArmsMesh(GetOwner()), FP_CharacterThrowMontage);
			CustomAnimationUtils::PlayMontage(FP_Mesh, FP_ThrowMontage);

			CustomAnimationUtils::PlayMontage(IGunHolder::Execute_GetTPMesh(GetOwner()), TP_CharacterThrowMontage);
			CustomAnimationUtils::PlayMontage(TP_Mesh, TP_CharacterThrowMontage);
		}
	}
}

bool AThrowableBase::IsLocallyHeld()
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

void AThrowableBase::SphereTrace()
{
	if (UWorld* World = GetWorld())
	{
		TArray<FHitResult> SweepResults;

		FVector ActorLocation = GetActorLocation();

		FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(InnerRadius);

		//DrawDebugSphere(World, ActorLocation, InnerRadius, 100, FColor::Green, true);

		if (World->SweepMultiByChannel(
			SweepResults,
			ActorLocation,
			ActorLocation,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel3,
			CollisionSphere
		))
		{

			TArray<AActor*> AllHitCharacters;

			// Add all hit characters in AllHitCharacters array
			for (auto& HitResult : SweepResults)
			{
				if (AActor* HitActor = HitResult.GetActor())
				{
					if (APlayableCharacter* Character = Cast<APlayableCharacter>(HitActor))
					{
						AllHitCharacters.Add(Character);
					}
				}
			}

			/*For each character hit by the sphere trace
			* let's check if there was some object between them.
			* We do a line trace from the pelvis of the character
			* to the center of the sphere trace (grenade explosion for example)
			* If there is nothing between them, we can say that the character was hit by this Throwable.
			*/
			for (auto& Character : AllHitCharacters)
			{
				TArray<FHitResult> OutHits;

				APlayableCharacter* Char = Cast<APlayableCharacter>(Character);

				FCollisionQueryParams QueryParams;

				QueryParams.AddIgnoredActor(this);
				QueryParams.AddIgnoredActors(AllHitCharacters);

				//	DrawDebugLine(World, Char->GetMesh()->GetBoneLocation(FName("pelvis")), ActorLocation, FColor::Magenta, true);

					//to avoid hitting the floor or the surface in the trace
					//ActorLocation.Z += 5.0f;

				if (!World->LineTraceMultiByChannel(OutHits, Char->GetMesh()->GetBoneLocation(FName("pelvis")), ActorLocation, ECollisionChannel::ECC_GameTraceChannel3, QueryParams))
				{
					UE_LOG(LogTemp, Warning, TEXT("Didnt hit anything"));
					Char->HitByExplosion(Cast<APlayableCharacter>(GetOwner()));
				}
				else
				{
					for (auto& OutHitResult : OutHits)
					{
						UE_LOG(LogTemp, Warning, TEXT("Hit something: %s"), *OutHitResult.GetActor()->GetName());
						if (!OutHitResult.GetActor()->IsA(APlayableCharacter::StaticClass()))
						{

							return;
						}
					}

					UE_LOG(LogTemp, Warning, TEXT("Hit other player! Grenade will hit."));

					Char->HitByExplosion(Cast<APlayableCharacter>(GetOwner()));
				}
			}
		}
	}
}

#pragma region IThrowableImplementation
void AThrowableBase::Throw_Implementation()
{
	if (GetOwner() && !bWasThrown)
	{
		if (GetOwner()->Implements<UGunHolder>())
		{
			FP_Mesh->SetOnlyOwnerSee(false);
			TP_Mesh->SetHiddenInGame(true);

			FP_Mesh->SetRelativeLocation(FVector::ZeroVector);
			TP_Mesh->SetRelativeLocation(FVector::ZeroVector);

			DetachFromActor(FDetachmentTransformRules::FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));

			FRotator ControlRotation = IGunHolder::Execute_GetHolderControlRotation(GetOwner());

			FVector ForwardVector = UKismetMathLibrary::GetForwardVector(ControlRotation);

			Velocity = ForwardVector * ThrowableScalarImpulse;

			bIsCurrentlyHeld = false;

			if (HasAuthority())
			{
				Multi_Throw(GetActorLocation(), ForwardVector);
			}
			else
			{
				Server_Throw(GetActorLocation(), ForwardVector);
			}

			bWasThrown = true;

		}
	}
}
void AThrowableBase::BeginThrow_Implementation()
{
	PlayThrowAnimation();

	if (!HasAuthority())
	{
		Server_BeginThrow();
	}
	else
	{
		Multi_BeginThrow();
	}
}
bool AThrowableBase::Server_BeginThrow_Validate()
{
	return true;
}
void AThrowableBase::Server_BeginThrow_Implementation()
{
	PlayThrowAnimation();

	Multi_BeginThrow();
}
bool AThrowableBase::Multi_BeginThrow_Validate()
{
	return true;
}
void AThrowableBase::Multi_BeginThrow_Implementation()
{
	if (!IsLocallyHeld())
	{
		PlayThrowAnimation();
	}
}
bool AThrowableBase::Server_Throw_Validate(FVector ThrowableLocation, FVector ForwardVector)
{
	return true;
}
void AThrowableBase::Server_Throw_Implementation(FVector ThrowableLocation, FVector ForwardVector)
{
	if (!IsLocallyHeld())
	{
		FP_Mesh->SetOnlyOwnerSee(false);
		TP_Mesh->SetHiddenInGame(true);

		FP_Mesh->SetRelativeLocation(FVector::ZeroVector);
		TP_Mesh->SetRelativeLocation(FVector::ZeroVector);

		DetachFromActor(FDetachmentTransformRules::FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));

		SetActorLocation(ThrowableLocation);

		Velocity = ForwardVector * ThrowableScalarImpulse;
		bWasThrown = true;

		bIsCurrentlyHeld = false;
	}
	Multi_Throw(ThrowableLocation, ForwardVector);
}
bool AThrowableBase::Multi_Throw_Validate(FVector ThrowableLocation, FVector ForwardVector)
{
	return true;
}
void AThrowableBase::Multi_Throw_Implementation(FVector ThrowableLocation, FVector ForwardVector)
{
	if (!IsLocallyHeld())
	{
		FP_Mesh->SetOnlyOwnerSee(false);
		TP_Mesh->SetHiddenInGame(true);

		FP_Mesh->SetRelativeLocation(FVector::ZeroVector);
		TP_Mesh->SetRelativeLocation(FVector::ZeroVector);

		DetachFromActor(FDetachmentTransformRules::FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));

		SetActorLocation(ThrowableLocation);
		Velocity = ForwardVector * ThrowableScalarImpulse;
		bWasThrown = true;
	}
}
#pragma endregion
