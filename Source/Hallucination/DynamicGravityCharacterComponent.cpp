// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicGravityCharacterComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"



void UDynamicGravityCharacterComponent::BeginPlay()
{
	Super::BeginPlay();
}

ACharacter* UDynamicGravityCharacterComponent::GetOwnerCharacter() const
{
	AActor* ownerActor = GetOwner();
	return Cast<ACharacter>(ownerActor);
}

void UDynamicGravityCharacterComponent::ApplyDynamicGravity(FVector Direction, float DeltaTime)
{
	TObjectPtr<UCharacterMovementComponent> movement = GetOwnerCharacter()->GetCharacterMovement();
	check(movement);

	FRotator&& gravityRotation = GetGravityRotator();
	GetOwnerPawn()->SetActorRotation(gravityRotation + ProjectToHorizontalPlane(GetGravityRotatedControllForward()).Rotation());

	FHitResult&& hit = GetDownLayHit();
	if (hit.bBlockingHit)
	{
		movement->MovementMode = EMovementMode::MOVE_Walking;
	}
	else
	{
		movement->MovementMode = EMovementMode::MOVE_Falling;
	}
	
	const UPrimitiveComponent* HitComponent = hit.Component.Get();
	if (HitComponent)
	{
		FHitResult dzHit;
		UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		const FBoxSphereBounds& bound = primitiveComp->Bounds;
		float traceLength = bound.BoxExtent.Z * 1.4f;
		const FVector&& dx = GetOwnerCharacter()->GetActorForwardVector() * GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector&& from = GetOwner()->GetActorLocation() + dx;
		const FVector&& to = from + GetGravityDirection() * traceLength;
		GetWorld()->LineTraceSingleByChannel(dzHit, from, to, ECollisionChannel::ECC_Visibility);
		//DrawDebugLine(GetWorld(), from, to, FColor::Green, false, -1.f, 0, 5.f);
		//DrawDebugLine(GetWorld(), dzHit.Location, dzHit.Location + dzHit.Normal * 50.f, FColor::Blue, false, -1.f, 0, 5.f);
		
		if (dzHit.bBlockingHit)
		{
			FVector&& tangentVec = dzHit.Location - hit.Location;
			tangentVec.Normalize();
			const float&& relativeSlopeAngleRad = FMath::Acos(tangentVec.Dot(GetOwnerCharacter()->GetActorForwardVector()));
			const float&& relativeSlopeAngleRadN = FMath::Acos(hit.Normal.Dot(-GetGravityDirection()));
			const float&& relativeSlopeAngle = relativeSlopeAngleRad / PI * 180.f;

			const bool&& walkable = movement->GetWalkableFloorAngle() >= relativeSlopeAngle;
			FVector&& slopeDown = hit.Normal.Cross(GetGravityDirection().Cross(hit.Normal));
			slopeDown.Normalize();
			//DrawDebugLine(GetWorld(), hit.Location, hit.Location + hit.Normal * 50.f, FColor::Red, false, -1.f, 0, 5.f);

			// Can't walk on this surface if it is too steep.
			if (!walkable)
			{
				Velocity += slopeDown * GetGravitationalAcceleration();
				FHitResult slopeDownHit;
				const FVector& slopeFrom = hit.Location;
				const FVector&& slopeTo = slopeFrom + slopeDown * 100.f;
				GetWorld()->LineTraceSingleByChannel(slopeDownHit, slopeFrom, slopeTo, ECollisionChannel::ECC_Visibility);

				const float& speed = movement->Velocity.Dot(slopeDown);
				GetOwnerCharacter()->AddActorWorldOffset(-slopeDown * speed * DeltaTime, true );
			}

			const float& speed = movement->Velocity.Dot(-slopeDown);
			const FVector&& offset = GetGravityDirection().Cross(tangentVec).Cross(tangentVec);
			//DrawDebugLine(GetWorld(), hit.Location, hit.Location + offset * bound.BoxExtent.Z, FColor::Cyan, false, -1.f, 0, 5.f);
			//DrawDebugLine(GetWorld(), hit.Location, hit.Location + tangentVec * 100.f, FColor::Emerald, false, -1.f, 0, 5.f);

			// Slope at the ceiling
			if (relativeSlopeAngle > 10.f && movement->IsWalkable(hit) != walkable)
			{
				FHitResult swept;
				swept.bStartPenetrating = true;
				const FVector&& dLoc = tangentVec * speed * DeltaTime;

				if (swept.bBlockingHit)
				{
					GetOwnerCharacter()->AddActorWorldOffset(dLoc * (swept.PenetrationDepth * KINDA_SMALL_NUMBER));
				}
			}
		}
	}

	if (movement->IsFalling())
	{
		Velocity += Direction * DeltaTime * GetGravitationalAcceleration();
	}
	else
	{	
		GetOwner()->AddActorWorldOffset(GetGravityDirection(), true);
		Velocity = FVector::Zero();
	}
	UpdateComponentVelocity();
	FVector MovementVelocity = movement->Velocity;
	MovementVelocity.Z = 0.0f;
	movement->Velocity = Velocity + MovementVelocity;
	movement->UpdateComponentVelocity();
}
