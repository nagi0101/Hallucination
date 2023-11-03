// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicGravityPawnComponent.h"
#include "DynamicGravityCharacterComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HALLUCINATION_API UDynamicGravityCharacterComponent : public UDynamicGravityPawnComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() final;

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterComponent")
	ACharacter* GetOwnerCharacter() const;

	//virtual void DynamicGravity_Implementation(FVector Direction, float DeltaTime) final;

	virtual void ApplyDynamicGravity(FVector Direction, float DeltaTime) final;
};
