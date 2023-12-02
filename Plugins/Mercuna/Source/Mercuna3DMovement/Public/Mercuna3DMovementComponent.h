// Copyright (C) 2016-2019 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Mercuna3DMovementInterface.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Mercuna3DMovementComponent.generated.h"

/**
* 3D Movement component for use with Mercuna
*/
UCLASS(ClassGroup = Mercuna, NotBlueprintable, hidecategories = (Object, Sockets, Collision, NavMovement), meta = (DisplayName = "Mercuna 3D Movement Component", BlueprintSpawnableComponent, ShortTooltip = "3d movement component for use with Mercuna"))
class MERCUNA3DMOVEMENT_API UMercuna3DMovementComponent : public UPawnMovementComponent, public IMercuna3DMovementInterface
{
    GENERATED_UCLASS_BODY()

    //Begin UActorComponent Interface
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
    //End UActorComponent Interface

    // Current velocity in local frame
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Movement")
    FVector GetLocalVelocity() const;

    // Explicitly set the current velocity of the movement component.
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Movement")
    void SetVelocity(FVector NewVelocity);

    // Explicitly set the current angular velocity of the movement component.
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Movement")
    void SetAngularVelocity(FVector NewAngVel);

    /** Add impulse relative to mass (delta V) in world frame. Impulses are
    * accumulated each tick and applied together. (Equivalent to Character 
    * Movement Component function AddImpulse with bVelocityChange set to true). */ 
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Movement")
    void AddImpulse(FVector Impulse);

    /** Set the current velocity and angular velocity of the movement component to the current physics velocities.
     * Use this before disabling physics. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Movement")
    void SetVelocitiesFromPhysics();

    virtual void StopMovementImmediately() override;

protected:
    virtual bool ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit, const FQuat& NewRotation) override;

    /** Properties defining the 3D movement capabilities of the pawn */
    UPROPERTY(Category = "3D Movement Properties", EditAnywhere, BlueprintReadOnly, meta=(ShowOnlyInnerProperties))
    FMercuna3DMovementProperties MovementProperties;

    /** Set to true when a position correction is applied. Used to avoid recalculating velocity when this occurs. */
    UPROPERTY(Transient)
    uint32 bPositionCorrected : 1;

    /** Accumulated impulse to be added next tick. */
    UPROPERTY()
    FVector PendingImpulseToApply;

public:

    /** Applies momentum accumulated through AddImpulse(), then clears those forces.  */
    virtual void ApplyAccumulatedImpulse();

    virtual void SetDesiredWorldVelocity(const FVector& DesiredWorldVelocity) override;

    void SetForwardAcceleration(float Magnitude);

    void SetLocalAcceleration(const FVector& DesiredLocalAcceleration);

    virtual void SetWorldAcceleration(const FVector& DesiredWorldAcceleration) override;

    void SetPitchRate(float Magnitude);

    void SetYawRate(float Magnitude);

    void SetRollRate(float Magnitude);

    virtual void SetDesiredLocalRotationRate(const FRotator& DesiredLocalRotationRate) override;

    void SetDesiredWorldOrientation(const FQuat& DesiredWorldOrientation);

    FORCEINLINE virtual const FMercuna3DMovementProperties& GetMovementProperties() const override { return MovementProperties; }

    // Update the movement properties. Any 3D navigation component will be automatically reconfigured
    // Note that any existing movement request will be cancelled when the navigation component is reconfigured.
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Movement")
    virtual void SetMovementProperties(const FMercuna3DMovementProperties& NewMovementProperties) override;

    // Get the pawn's rate of pitch, yaw and roll, in degrees/sec in the pawn's local frame
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Movement")
    virtual FRotator GetLocalRotationRate() const override;

    // Get the current angular velocity in the actor's local frame
    FORCEINLINE FVector GetLocalAngularVelocity() const { return AngularVelocity; }

    // Get the current linear acceleration in the actor's local frame
    FORCEINLINE FVector GetLocalAcceleration() const { return Acceleration; }

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR

protected:
    void SetRotationRatesForDesiredOrientation(float DeltaTime);

    // Current acceleration in actor's local frame
    UPROPERTY(BlueprintReadOnly, Transient, Category="3D Movement Properties")
    FVector Acceleration;

    // Current angular velocity in local frame
    UPROPERTY(BlueprintReadOnly, Transient, Category="3D Movement Properties")
    FVector AngularVelocity;

    // Desired angular velocity from stick inputs, in local frame
    FVector DesiredAngularVelocity;

    // Desired orientation, in world frame, used as an alternative to desired angular velocity.
    FQuat DesiredWorldOrientation;
};
