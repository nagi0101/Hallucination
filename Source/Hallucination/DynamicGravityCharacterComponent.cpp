// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicGravityCharacterComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"



void UDynamicGravityCharacterComponent::BeginPlay()
{
	Super::BeginPlay();
}

ACharacter* UDynamicGravityCharacterComponent::GetOwnerCharacter() const
{
	AActor* ownerActor = GetOwner();
	return Cast<ACharacter>(ownerActor);
}

void UDynamicGravityCharacterComponent::DynamicGravity_Implementation(FVector Direction, float DeltaTime)
{
	ApplyDynamicGravity(Direction, DeltaTime);

	TObjectPtr<UCharacterMovementComponent> movement = GetOwnerCharacter()->GetCharacterMovement();

	FHitResult hit = GetDownLayHit();
	if (hit.bBlockingHit)
	{
		movement->MovementMode = EMovementMode::MOVE_Walking;
	}
	else
	{
		movement->MovementMode = EMovementMode::MOVE_Falling;
	}
}

void UDynamicGravityCharacterComponent::ApplyDynamicGravity(FVector Direction, float DeltaTime)
{
	TObjectPtr<UCharacterMovementComponent> movement = GetOwnerCharacter()->GetCharacterMovement();
	check(movement);

	FRotator gravityRotation = GetGravityRotator();
	GetOwnerPawn()->SetActorRotation(gravityRotation + ProjectToHorizontalPlane(GetGravityRotatedControllForward()).Rotation());

	FHitResult hit = GetDownLayHit();
	if (hit.bBlockingHit)
	{
		movement->MovementMode = EMovementMode::MOVE_Walking;
	}
	else
	{
		movement->MovementMode = EMovementMode::MOVE_Falling;
	}

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
