// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicGravityActorComponent.h"
#include "DynamicGravityPawnComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HALLUCINATION_API UDynamicGravityPawnComponent : public UDynamicGravityActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "PawnComponent")
	APawn* GetOwnerPawn() const;

	virtual void DynamicGravity_Implementation(FVector Direction, float DeltaTime) override;

	virtual void SetupTargetComponent() override;
	virtual void ApplyDynamicGravity(FVector Direction, float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	FVector GetGravityRotatedControllForward() const;

	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	FVector GetGravityRotatedControllRight() const;
};
