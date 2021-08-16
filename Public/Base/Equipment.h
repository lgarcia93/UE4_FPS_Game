// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "TacticalShooter/Public/Enums/Enums.h"
#include "Equipment.generated.h"

UCLASS()
class TACTICALSHOOTER_API AEquipment : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(Replicated)
		bool bIsCurrentlyHeld = false;

	UPROPERTY(EditAnywhere, Category = "Equipment Info")
		FName Description;

	UPROPERTY(EditAnywhere, Category = "Equipment Info")
		EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, Category = "Equipment Info")
		EEquipable EnumIdentifier;

	UPROPERTY(EditAnywhere, Category = "Equipment Info")
		FName SocketToAttach;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Mesh")
		USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Mesh")
		USkeletalMeshComponent* TP_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* FP_EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* FP_CharacterEquipMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* TP_EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* TP_CharacterEquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* FP_UnequipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* FP_CharacterUnequipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* TP_UnequipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Animations")
		UAnimMontage* TP_CharacterUnequipMontage;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Sets default values for this actor's properties
	AEquipment();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FName GetDescription();

	UFUNCTION(BlueprintCallable)
		void Equip();

	UFUNCTION(BlueprintCallable)
		void Unequip(bool PlayMontage = false);

	UFUNCTION(BlueprintCallable)
		void Drop();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsCurrentlyHeld();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		EEquipable GetEnumIdentifier();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		EEquipmentType GetEquipmentType();

	UFUNCTION(BlueprintCallable)
		void HideInGame();

	UFUNCTION(BlueprintCallable)
		void ShowInGame();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		inline USkeletalMeshComponent* GetFPMesh() { return FP_Mesh; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
		inline USkeletalMeshComponent* GetTPMesh() { return TP_Mesh; };
};
