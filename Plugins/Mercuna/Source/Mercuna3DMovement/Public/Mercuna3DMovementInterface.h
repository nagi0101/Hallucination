// Copyright (C) 2016 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/Interface.h"

/**
* Standardized movement component interface that allows the Mercuna3DMovement component to be 
* replaced by alternative movement components if they implement this interface
*/
#include "Mercuna3DMovementInterface.generated.h"

USTRUCT(BlueprintType)
struct FMercuna3DMovementAccelerations
{
    GENERATED_USTRUCT_BODY()

    /** Maximum acceleration in the forward direction */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mercuna")
    float Forward;

    /** Maximum acceleration in the backwards direction */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mercuna")
    float Backward;

    /** Maximum acceleration in the horizontal and vertical direction */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mercuna")
    float Sideways;

    /** defaults */
    FMercuna3DMovementAccelerations()
    {
        Forward = 300.0f;
        Backward = 300.0f;
        Sideways = 150.0f;
    }
};

USTRUCT(BlueprintType)
struct FMercuna3DMovementWorldAccelerations
{
    GENERATED_USTRUCT_BODY()

    /** Maximum acceleration in the vertical up direction */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mercuna")
    float Upward;

    /** Maximum acceleration in the vertical down direction */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mercuna")
    float Downward;
    
    /** defaults */
    FMercuna3DMovementWorldAccelerations()
    {
        Upward = 300.0f;
        Downward = 300.0f;
    }
};

USTRUCT(BlueprintType)
struct FMercuna3DMovementProperties
{
    GENERATED_USTRUCT_BODY()

    /** Maximum speed */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3D Movement Properties")
    float MaxSpeed;

    /** Maximum accelerations in principle directions local to the agents frame */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D Movement Properties")
    FMercuna3DMovementAccelerations MaxAccelerations;

    /** Maximum pitch rate in radians/s. Clamped to within 10x yaw rate */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "3D Movement Properties")
    float MaxPitchRate;

    /** Maximum yaw rate in radians/s. Clamped to within 10x pitch rate */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D Movement Properties")
    float MaxYawRate;

    /** Maximum roll rate in radians/s */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D Movement Properties")
    float MaxRollRate;

    /** Maximum angular acceleration in radians/s/s */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3D Movement Properties")
    float MaxAngAccel;

    // Enable world frame acceleration limits
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "3D Movement Properties", meta = (InlineEditConditionToggle))
    bool bEnableWorldAccelerationLimits;

    /** Additional maximum accelerations limits in world frame. Only applied if enabled  */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "3D Movement Properties", meta = (EditCondition = bEnableWorldAccelerationLimits))
    FMercuna3DMovementWorldAccelerations WorldAccelerationLimits;

    /** defaults */
    FMercuna3DMovementProperties()
    {
        MaxSpeed = 500.f;
        MaxPitchRate = 1.5f;
        MaxYawRate = 1.5f;
        MaxRollRate = 1.5f;
        MaxAngAccel = 2.5f;
        bEnableWorldAccelerationLimits = false;
    }
};

DECLARE_MULTICAST_DELEGATE(FMercuna3DMovementPropertiesChanged);

UINTERFACE(NotBlueprintable)
class MERCUNA3DMOVEMENT_API UMercuna3DMovementInterface : public UInterface
{
    GENERATED_UINTERFACE_BODY()
};

class MERCUNA3DMOVEMENT_API IMercuna3DMovementInterface
{
    GENERATED_IINTERFACE_BODY()

public:
    // Delegate that will be triggered whenever one of the movement properties changes
    FMercuna3DMovementPropertiesChanged OnMovementPropertiesChanged;
    
    // Set desired velocity, in world frame
    virtual void SetDesiredWorldVelocity(const FVector& DesiredWorldVelocity) = 0;

    // Set acceleration, in world frame
    virtual void SetWorldAcceleration(const FVector& DesiredWorldAcceleration) = 0;

    // Set desired rate of pitch, yaw and roll, in degrees/sec in the pawn's local frame
    virtual void SetDesiredLocalRotationRate(const FRotator& DesiredLocalRotationRate) = 0;

    // Query the movement configuration properties
    virtual const FMercuna3DMovementProperties& GetMovementProperties() const = 0;

    // Set the movement configuration properties
    virtual void SetMovementProperties(const FMercuna3DMovementProperties& NewMovementProperties) = 0;

    // Query the current rate of pitch, yaw and roll, in degrees/sec in the pawn's local frame
    virtual FRotator GetLocalRotationRate() const = 0;
};
