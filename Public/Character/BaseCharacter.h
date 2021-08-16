#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "TacticalShooter/Public/Interfaces/GunHolder.h"
#include "BaseCharacter.generated.h"

UCLASS()
class TACTICALSHOOTER_API ABaseCharacter : public ACharacter, public IGunHolder
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* ThirdPersonMeshLegs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpringArmComponent* SpringArmComponent;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		USkeletalMeshComponent* GetFPArmsMesh();

	virtual USkeletalMeshComponent* GetFPArmsMesh_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		USkeletalMeshComponent* GetTPMesh();

	virtual USkeletalMeshComponent* GetTPMesh_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		FRotator GetHolderControlRotation();

	virtual FRotator GetHolderControlRotation_Implementation() override;

	//virtual FRotator* GetTPMesh() override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsLocallyHeld();

	virtual bool IsLocallyHeld_Implementation() override;

protected:
	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsSprinting;

	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsADS;

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	///** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return FirstPersonMesh; }
	///** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCamera; }
};
