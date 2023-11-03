// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "Components/ArrowComponent.h"
#include "HallucinationCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class USpringArmComponent;

struct FDynamicMaterialScalarProperty
{
	FName Name;
	float Value;
};

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS(config=Game)
class AHallucinationCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* InteractionSpot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

private:
	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float controllerPitchMin;

	/** Movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool IsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stemina, meta = (AllowPrivateAccess = "true"))
	float MaxStemina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stemina, meta = (AllowPrivateAccess = "true"))
	float Stemina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stemina, meta = (AllowPrivateAccess = "true"))
	float SteminaConsumptionBreath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stemina, meta = (AllowPrivateAccess = "true"))
	float SteminaConsumptionRun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stemina, meta = (AllowPrivateAccess = "true"))
	float SteminaRecoveryRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stemina, meta = (AllowPrivateAccess = "true"))
	bool IsExhaused;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stemina, meta = (AllowPrivateAccess = "true"))
	float SteminaRecoveryThreshold;

	/** Breath */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Breath, meta = (AllowPrivateAccess = "true"))
	bool IsHoldingBreath;

	/** HP */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HP, meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HP, meta = (AllowPrivateAccess = "true"))
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HP, meta = (AllowPrivateAccess = "true"))
	float HPRecoveryRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HP, meta = (AllowPrivateAccess = "true"))
	float HPRecoveryCooltime;

	UPROPERTY(BlueprintReadOnly, Category = HP, meta = (AllowPrivateAccess = "true"))
	float LastDamaged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HP, meta = (AllowPrivateAccess = "true"))
	bool isDead;

	/* Interact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	bool IsGrabbing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	bool OnPushingAndPulling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	FVector PushPullDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	double DistToObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	float InteractDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	AActor* interactedObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* interactedComp;

	/* Skill */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	bool canControlGravitiy;

	/** Camera Shake */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraShake, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> CS_Idle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraShake, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> CS_Walk;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraShake, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> CS_Run;

	/** Post Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PostProcessMaterial, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* M_Vinyette;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* MD_Vinyette;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PostProcessMaterial, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* M_Blood;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* MD_Blood;

	/** Sound Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_Inhale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_ExhaleStrong;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_Exhale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_PickUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_Putdown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_Drag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_DrinkPotion;

public:
	AHallucinationCharacter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HP)
	void Damage(float damage);

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

private:
	UFUNCTION(BlueprintCallable, Category=CameraShake)
	void SetCameraShake(FVector velocity);

	UFUNCTION(BlueprintCallable, Category = Movement)
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = Stemina)
	void CheckStemina(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = Movement)
	void EndSprint();

	UFUNCTION(BlueprintCallable, Category = Breath)
	void StartHoldBreath();

	UFUNCTION(BlueprintCallable, Category = Breath)
	void EndHoldBreath();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	bool LineTrace(FHitResult& hit, float dis);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void Interact();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void Throw();

	void Pickup();

	void Putdown();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void PushAndPull(float scale);

	UFUNCTION(BlueprintCallable, Category = PostProcess)
	void SetPostProcessParameter();

	void SetPostProcessScalarParameters(UMaterialInstanceDynamic*& DynamicMaterial, TArray<FDynamicMaterialScalarProperty>& PropertiesInfo);

	void SetPostProcessMaterialInstance(UMaterialInterface*& Material, UMaterialInstanceDynamic** DynamicMaterialOut, float weight = 1.0f);

	UFUNCTION(BlueprintCallable, Category = HP)
	void CheckHP(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = HP)
	void Die();

	UFUNCTION(BlueprintCallable, Category = HP)
	void Revive();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;
protected:
	
	/** Fires a projectile. */
	void OnPrimaryAction();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
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

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};
