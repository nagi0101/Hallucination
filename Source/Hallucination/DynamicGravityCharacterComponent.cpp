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

//void UDynamicGravityCharacterComponent::DynamicGravity_Implementation(FVector Direction, float DeltaTime)
//{
//	ApplyDynamicGravity(Direction, DeltaTime);
//
//	TObjectPtr<UCharacterMovementComponent> movement = GetOwnerCharacter()->GetCharacterMovement();
//
//	FHitResult hit = GetDownLayHit();
//	//UE_LOG(LogTemp, Warning, TEXT("%d"), movement->IsFalling())
//	if (hit.bBlockingHit)
//	{
//		movement->MovementMode = EMovementMode::MOVE_Walking;
//	}
//	else
//	{
//		movement->MovementMode = EMovementMode::MOVE_Falling;
//	}
//}

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
	
	const UPrimitiveComponent* HitComponent = hit.Component.Get();
	bool walkable = true;
	FVector slopeDown;
	float relativeSlopeAngle = 0.0f;
	if (HitComponent)
	{
		float relativeSlopeAngleRad = FMath::Acos(hit.Normal.Dot(-GetGravityDirection()));
		relativeSlopeAngle = relativeSlopeAngleRad / PI * 180.f;

		walkable = movement->GetWalkableFloorAngle() >= relativeSlopeAngle;
		slopeDown = hit.Normal.Cross(GetGravityDirection().Cross(hit.Normal));
		slopeDown.Normalize();
	}
		// Can't walk on this surface if it is too steep.
		if (!walkable)
		{
			UE_LOG(LogTemp, Warning, TEXT("CAN'T WALK"));
			Velocity += slopeDown * GetGravitationalAcceleration();
			FHitResult slopeDownHit;
			float traceLength = 100.f;
			FVector from = hit.Location;
			FVector to = from + slopeDown * traceLength;
			GetWorld()->LineTraceSingleByChannel(slopeDownHit, from, to, ECollisionChannel::ECC_Visibility);

			float speed = movement->Velocity.Dot(slopeDown);
			GetOwnerCharacter()->AddActorWorldOffset(-slopeDown * speed * DeltaTime, true);
			//GetOwnerCharacter()->SetActorLocation(slopeDownHit.Location, true);
		}

		// inv slope
		if (relativeSlopeAngle > KINDA_SMALL_NUMBER && movement->IsWalkable(hit) != walkable)
		{
			UE_LOG(LogTemp, Warning, TEXT("inv slope"));
			if (relativeSlopeAngle > 10.f)
			{
				float speed = movement->Velocity.Dot(slopeDown);
				GetOwnerCharacter()->AddActorWorldOffset(slopeDown * speed * DeltaTime, true);
			}
		}

	if (movement->IsFalling())
	{
		Velocity += Direction * DeltaTime * GetGravitationalAcceleration();
	}
	else
	{
		Velocity = Direction * GetGravitationalAcceleration();
	}
	UpdateComponentVelocity();
	movement->Velocity += Velocity;
	movement->UpdateComponentVelocity();
}
