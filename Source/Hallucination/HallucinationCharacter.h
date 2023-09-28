// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "HallucinationCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

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

private:
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Breath, meta = (AllowPrivateAccess = "true"))
	bool IsHoldingBreath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability, meta = (AllowPrivateAccess = "true"))
	bool IsGrabbing;

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

	/** Sound Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_Inhale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_ExhaleStrong;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SoundEffect, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_Exhale;

public:
	AHallucinationCharacter();

protected:
	virtual void BeginPlay();

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

	UFUNCTION(BlueprintCallable, Category = Ability)
	FHitResult Pickup();

	UFUNCTION(BlueprintCallable, Category = Ability)
	void Throw();

	UFUNCTION(BlueprintCallable, Category = PostProcess)
	void SetPostProcessParameter();

	UFUNCTION(BlueprintCallable, Category = PostProcess)
	void SetPostProcessMaterialInstance(UMaterialInterface*& Material, UMaterialInstanceDynamic*& DynamicMaterial);

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

