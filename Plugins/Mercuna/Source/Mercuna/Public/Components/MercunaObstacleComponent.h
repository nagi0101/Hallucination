// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/SceneComponent.h"

#include "Runtime/Launch/Resources/Version.h"

#include "MercunaObstacleComponent.generated.h"

UENUM()
enum class EMercunaObstacleType : uint8
{
    Sphere         UMETA(DisplayName = "Sphere"),
    Box            UMETA(DisplayName = "Box"),
    Cylinder       UMETA(DisplayName = "Cylinder"),
};

UENUM()
enum class EMercunaObstacleMovementType : uint8
{
    ThreeDimensional    UMETA(DisplayName = "3D"),
    Character           UMETA(DisplayName = "Character"),
    Vehicle             UMETA(DisplayName = "Vehicle"),
    None                UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EMercunaObstacleReaction : uint8
{
    Avoid               UMETA(DisplayName = "Avoid"),
    DoNotAvoid          UMETA(DisplayName = "Don't avoid"),
};

/** A component that registers an actor as a Mercuna dynamic obstacle */
UCLASS(ClassGroup = Mercuna, NotBlueprintable, hideCategories = (Tags, Physics, Collision, Lighting, Rendering, Mobile, Sockets), meta = (BlueprintSpawnableComponent, ShortTooltip = "A component to register the actor as a Mercuna dynamic obstacle"))
class MERCUNA_API UMercunaObstacleComponent : public USceneComponent
{
    GENERATED_UCLASS_BODY()

    // Is the sphere automatically calculated based on actor's components 
    // bounding boxes, or is it manually specified 
    UPROPERTY(EditAnywhere, Category = Obstacle)
    bool bAutomatic;

    // What shape to treat the actor as. Ignored in Automatic mode
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle)
    EMercunaObstacleType Type;

    // If a sphere obstacle, the radius of the obstacle. Ignored in Automatic mode
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle, meta = (ClampMin = "0.1"))
    float SphereRadius;

    // If a box obstacle, the size of the box. Ignored in Automatic mode
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle)
    FVector BoxSize;

    // If a cylinder obstacle, the radius of the obstacle. Ignored in Automatic mode
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle, meta = (ClampMin = "0.1"))
    float CylinderRadius;

    // If a cylinder obstacle, the height of the obstacle. Ignored in Automatic mode
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle, meta = (ClampMin = "0.1"))
    float CylinderHeight;

    // How avoidance expects this obstacle to move. Ignored in Automatic mode
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle)
    EMercunaObstacleMovementType MovementType;

    // How should other pawns treat this obstacle by default, unless they
    // are told to explicitly do otherwise (via SetAvoidanceAgainst).
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle, AdvancedDisplay)
    EMercunaObstacleReaction DefaultReaction;

    // When stationary should this obstacle be cut out of the nav grid, so that
    // paths will then navigate around it.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle)
    bool bCutOutWhenStationary;

    // Time in seconds the obstacle must be stopped for until it is considered stationary
    // and cut out of nav grids. 
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Obstacle, meta = (ClampMin = "0.1", EditCondition="bCutOutWhenStationary"))
    float StationaryThresholdTime;

    // Update how avoidance expects this obstacle to move
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Obstacle")
    void UpdateMovementType(const EMercunaObstacleMovementType& NewMovementType);

    // Begin UActorComponent interface
    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
    // End UActorComponent interface

    //~ Begin USceneComponent Interface
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    //~ End USceneComponent Interface

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual bool CanEditChange(const UProperty* Property) const override;
#else
    virtual bool CanEditChange(const FProperty* Property) const override;
#endif
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditComponentMove(bool bFinished);
#endif

    // Calculate the world space size of an actor based on collision components
    static float CalculateRadius(AActor* Actor);
    static void CalculateRadiusAndPosition(AActor* Actor, float& Radius, FVector& WorldPosition);

protected:
    // Based on other components and collision work out the best shape for this obstacle
    virtual void CalculateAutomaticShape();

    // Use the actor's movement component to work out the movement type
    virtual EMercunaObstacleMovementType CalculateMovementType() const;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;

    virtual void Activate(bool bReset = false) override;
    virtual void Deactivate() override;

private:
    UFUNCTION()
    void OnCharacterMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

    // Get the actor aligned bounding box of the component
    static FBox GetBoundingBox(const UPrimitiveComponent* component, const FTransform& WorldToActor);
};
