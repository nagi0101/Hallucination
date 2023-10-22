// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicGravityPawnComponent.h"

#include "GameFramework/PawnMovementComponent.h"


void UDynamicGravityPawnComponent::BeginPlay()
{
	Super::BeginPlay();
}

APawn* UDynamicGravityPawnComponent::GetOwnerPawn() const
{
	AActor* ownerActor = GetOwner();
	APawn* ownerPawn = Cast<APawn>(ownerActor);
	return ownerPawn;
}

void UDynamicGravityPawnComponent::DynamicGravity_Implementation(FVector Direction, float DeltaTime)
{
	ApplyDynamicGravity(Direction, DeltaTime);
}

void UDynamicGravityPawnComponent::SetupTargetComponent()
{
}

void UDynamicGravityPawnComponent::ApplyDynamicGravity(FVector Direction, float DeltaTime)
{
	TObjectPtr<UPawnMovementComponent> movement = GetOwnerPawn()->GetMovementComponent();
	if (movement->IsFalling())
	{
		Velocity += Direction * DeltaTime * GetGravitationalAcceleration();
		UpdateComponentVelocity();

		movement->Velocity += Velocity;
		movement->UpdateComponentVelocity();
	}
	else
	{
		Velocity = FVector::Zero();
	}
}

FVector UDynamicGravityPawnComponent::GetGravityRotatedControllForward() const
{
	FVector controllForward = GetOwnerPawn()->GetControlRotation().Vector();
	return GetGravityRotator().RotateVector(controllForward);
}

FVector UDynamicGravityPawnComponent::GetGravityRotatedControllRight() const
{
	FVector controllRight = GetOwnerPawn()->GetControlRotation().RotateVector(FVector::RightVector);
	return GetGravityRotator().RotateVector(controllRight);
}