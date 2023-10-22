// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "DynamicGravityActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HALLUCINATION_API UDynamicGravityActorComponent : public UMovementComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDynamicGravityActorComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DynamicGravity")
	void DynamicGravity(FVector Direction, float DeltaTime);
	virtual void DynamicGravity_Implementation(FVector Direction, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	void SetGravityDirection(FVector Direction);

	virtual void SetupTargetComponent();
	virtual void ApplyDynamicGravity(FVector Direction, float DeltaTime);

	FVector GetGravityDirection() const;

	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	FRotator GetGravityRotator() const;

	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	FVector ProjectToHorizontalPlane(FVector in) const;
	
	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	FHitResult GetDownLayHit() const;

	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	FHitResult GetUpLayHit() const;

	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	const float GetMass() const;

	UFUNCTION(BlueprintCallable, Category = "DynamicGravity")
	const float GetGravitationalAcceleration() const { return GravitationalAcceleration; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicGravity", meta = (AllowPrivateAccess = "true"))
	FVector GravityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicGravity", meta = (AllowPrivateAccess = "true"))
	float GravitationalAcceleration;
};
