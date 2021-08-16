// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalShooter/Public/Character/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
// Sets default values
ABaseCharacter::ABaseCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	RootComponent = GetCapsuleComponent();

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));	
	FirstPersonCamera->SetupAttachment(SpringArmComponent);
	FirstPersonCamera->SetRelativeLocation(FVector(-15.881313f, 1.750031f, 74.669685f)); // Position the camera
	FirstPersonCamera->bUsePawnControlRotation = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	FirstPersonMesh->SetRelativeRotation(FRotator(1.899995f, -90.0f, 5.200002f));
	FirstPersonMesh->SetRelativeLocation(FVector(-10.194395f, -2.083456f, -164.176773f));
	GetMesh()->SetRelativeLocation(FVector(-8.21228f, 0.0f, -83.452324f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));	

	FirstPersonMesh->SetOnlyOwnerSee(true);
}

void ABaseCharacter::GetLifetimeReplicatedProps
(
	TArray < FLifetimeProperty >& OutLifetimeProps
) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		
	DOREPLIFETIME(ABaseCharacter, bIsADS);
	DOREPLIFETIME(ABaseCharacter, bIsSprinting);
}

USkeletalMeshComponent* ABaseCharacter::GetFPArmsMesh_Implementation()
{
	return GetMesh1P();
}

USkeletalMeshComponent* ABaseCharacter::GetTPMesh_Implementation()
{
	return GetMesh();
}

FRotator ABaseCharacter::GetHolderControlRotation_Implementation()
{
	return GetControlRotation();
}

bool ABaseCharacter::IsLocallyHeld_Implementation()
{
	return IsLocallyControlled();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseCharacter::LookUpAtRate);
}

void ABaseCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		
		if (bIsSprinting && Value < 0.0)
			return;

		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ABaseCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
//
//void ABaseCharacter::OnSprint()
//{
//	if (!bIsADS && !bIsCrouching)
//	{
//		bIsSprinting = true;
//	}
//}
//
//void ABaseCharacter::OnStopSprint()
//{
//	if (bIsSprinting)
//	{
//		bIsSprinting = false;
//	}
//}
//
//
//void ABaseCharacter::OnADS()
//{
//	if (!bIsSprinting)
//	{
//		bIsADS = true;
//	}
//}
//
//void ABaseCharacter::OnStopADS()
//{
//	bIsADS = false;
//}
//
//
//void ABaseCharacter::OnCrouch()
//{
//	Super::Crouch();
//}
//
//void ABaseCharacter::OnUnCrouch()
//{
//	Super::UnCrouch();
//}