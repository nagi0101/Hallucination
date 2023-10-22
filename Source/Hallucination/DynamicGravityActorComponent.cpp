// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicGravityActorComponent.h"

// Sets default values for this component's properties
UDynamicGravityActorComponent::UDynamicGravityActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	SetComponentTickEnabled(true);

	Activate();
	SetAutoActivate(true);

	GravityDirection = { 0.0f, 0.0f, -1.0f };
	GravitationalAcceleration = 980.f;
}


// Called when the game starts
void UDynamicGravityActorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupTargetComponent();

	Velocity = GravityDirection * GravitationalAcceleration;
}


// Called every frame
void UDynamicGravityActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DynamicGravity(GetGravityDirection(), DeltaTime);
}


void UDynamicGravityActorComponent::DynamicGravity_Implementation(FVector Direction, float DeltaTime)
{
	ApplyDynamicGravity(Direction, DeltaTime);
}

void UDynamicGravityActorComponent::SetGravityDirection(FVector Direction)
{
	GravityDirection = Direction;
	GravityDirection.Normalize();
}

void UDynamicGravityActorComponent::SetupTargetComponent()
{
	UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass())[0]);
	GetOwner()->SetRootComponent(primitiveComp);
	SetUpdatedComponent(GetOwner()->GetRootComponent());
	primitiveComp->SetSimulatePhysics(true);
	primitiveComp->SetEnableGravity(false);
}

void UDynamicGravityActorComponent::ApplyDynamicGravity(FVector Direction, float DeltaTime)
{
	UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	check(primitiveComp);

	if (GetDownLayHit().bBlockingHit)
	{
		Velocity = Direction * GetGravitationalAcceleration();
	}

	Velocity += Direction * DeltaTime * GetGravitationalAcceleration();
	UpdateComponentVelocity();
	primitiveComp->AddForce(Velocity * GetMass());
}

FVector UDynamicGravityActorComponent::GetGravityDirection() const
{
	return GravityDirection;
}

FRotator UDynamicGravityActorComponent::GetGravityRotator() const
{
	return GetGravityDirection().Rotation() - FVector::DownVector.Rotation();
}

FVector UDynamicGravityActorComponent::ProjectToHorizontalPlane(FVector in) const
{
	FVector gravity = GetGravityDirection();
	FVector toRight = gravity.Cross(in);
	FVector projected = toRight.Cross(gravity);
	projected.Normalize();
	return projected;
}

FHitResult UDynamicGravityActorComponent::GetDownLayHit() const
{
	UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	check(primitiveComp);

	FHitResult hit;
	FBoxSphereBounds bound = primitiveComp->Bounds;
	float traceLength = bound.BoxExtent.Z * 1.1f;
	FVector from = primitiveComp->GetComponentLocation();
	FVector to = from + GravityDirection * traceLength;
	GetWorld()->LineTraceSingleByChannel(hit, from, to, ECollisionChannel::ECC_Visibility);

	//DrawDebugLine(GetWorld(), from, to, FColor::Blue, false, 0.01f, 0, 5.f);

	return hit;
}

FHitResult UDynamicGravityActorComponent::GetUpLayHit() const
{
	UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	check(primitiveComp);

	FHitResult hit;
	FBoxSphereBounds bound = primitiveComp->GetLocalBounds();
	float traceLength = bound.BoxExtent.Z + 0.1f;
	FVector from = primitiveComp->GetComponentLocation();
	FVector to = from + -GravityDirection * traceLength;
	GetWorld()->LineTraceSingleByChannel(hit, from, to, ECollisionChannel::ECC_Visibility);

	return hit;
}

const float UDynamicGravityActorComponent::GetMass() const
{
	UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	check(primitiveComp);

	return primitiveComp->GetMass();
}
