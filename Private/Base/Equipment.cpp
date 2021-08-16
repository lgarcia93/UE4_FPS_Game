// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/Base/Equipment.h"
#include "Net/UnrealNetwork.h"
#include "TacticalShooter/Public/Interfaces/GunHolder.h"
#include "TacticalShooter/Public/Animation/CustomAnimationUtils.h"

// Sets default values
AEquipment::AEquipment()
{
// USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("Root");
	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("FP_Mesh");
	TP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("TP_Mesh");

	// RootComponent = Root;
	RootComponent = FP_Mesh;

	FP_Mesh->bCastDynamicShadow = false;
	FP_Mesh->CastShadow = false;

	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetOwnerNoSee(false);

	TP_Mesh->SetOnlyOwnerSee(false);
	TP_Mesh->SetOwnerNoSee(true);

	SetReplicates(true);

	PrimaryActorTick.bCanEverTick = true;

	FP_Mesh->SetRelativeLocation(FVector::ZeroVector);
	TP_Mesh->SetRelativeLocation(FVector::ZeroVector);
}


// Called when the game starts or when spawned
void AEquipment::BeginPlay()
{
	Super::BeginPlay();
}

void AEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquipment, bIsCurrentlyHeld);
}


FName AEquipment::GetDescription()
{
	return Description;
}

void AEquipment::Equip()
{
	if (GetOwner())
	{
		if (GetOwner()->Implements<UGunHolder>())
		{
			bIsCurrentlyHeld = true;

			FP_Mesh->SetSimulatePhysics(false);
			TP_Mesh->SetSimulatePhysics(false);
			FP_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			TP_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			FP_Mesh->AttachToComponent(IGunHolder::Execute_GetFPArmsMesh(GetOwner()), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketToAttach);

			TP_Mesh->AttachToComponent(IGunHolder::Execute_GetTPMesh(GetOwner()), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketToAttach);

			CustomAnimationUtils::PlayMontage(IGunHolder::Execute_GetFPArmsMesh(GetOwner()), FP_CharacterEquipMontage);
			CustomAnimationUtils::PlayMontage(FP_Mesh, FP_EquipMontage);
		}
	}
}

void AEquipment::Unequip(bool PlayMontage)
{
	CustomAnimationUtils::PlayMontage(IGunHolder::Execute_GetFPArmsMesh(GetOwner()), FP_CharacterUnequipMontage);
	CustomAnimationUtils::PlayMontage(FP_Mesh, FP_UnequipMontage);

	CustomAnimationUtils::PlayMontage(IGunHolder::Execute_GetTPMesh(GetOwner()), TP_CharacterUnequipMontage);
	CustomAnimationUtils::PlayMontage(TP_Mesh, TP_UnequipMontage);
}

void AEquipment::Drop()
{
	DetachFromActor(FDetachmentTransformRules::FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));

	//FP Mesh
	FP_Mesh->SetCollisionProfileName(FName("Custom"));
	FP_Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	FP_Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	FP_Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FP_Mesh->SetSimulatePhysics(true);

	//TP Mesh
	TP_Mesh->SetCollisionProfileName(FName("Custom"));
	TP_Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	TP_Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	TP_Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TP_Mesh->SetSimulatePhysics(true);

	bIsCurrentlyHeld = false;
}

bool AEquipment::IsCurrentlyHeld()
{
	return bIsCurrentlyHeld;
}

EEquipable AEquipment::GetEnumIdentifier()
{
	return EnumIdentifier;
}

EEquipmentType AEquipment::GetEquipmentType()
{
	return EquipmentType;
}

void AEquipment::HideInGame()
{
	SetHidden(true);
}

void AEquipment::ShowInGame()
{
	SetHidden(false);
}

