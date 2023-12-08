// Copyright (C) 2020-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "MerVolumeComponent.generated.h"

/**
 * Represents a generic cuboid volume
 */
UCLASS(ClassGroup = Mercuna, hideCategories = (Tags, Activation, Physics, Collision, Lighting, Rendering, Mobile, Sockets))
class UMerVolumeComponent : public UPrimitiveComponent
{
    GENERATED_UCLASS_BODY()

    DECLARE_DELEGATE(FOnMoved)
    
public:
    // Delegate called when component is moved in a game world
    FOnMoved OnMoved;

    void SetSize(FVector NewSize);
    FORCEINLINE FVector GetSize() const { return Size; }

    //~ Begin UPrimitiveComponent Interface.
    virtual bool IsZeroExtent() const override;
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    //~ End UPrimitiveComponent Interface.

    //~ Begin USceneComponent Interface
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    //~ End USceneComponent Interface

#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
    // The length of each edge of the volume
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    FVector Size;

    //~ Begin USceneComponent Interface
    virtual bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* Hit = nullptr, EMoveComponentFlags MoveFlags = MOVECOMP_NoFlags, ETeleportType Teleport = ETeleportType::None) override;
    //~ End USceneComponent Interface

public:
    UE_DEPRECATED(2.0, "Used for migration from 1.11 to 2.0")
    UPROPERTY(meta = (DeprecatedProperty))
    TObjectPtr<class AMercunaNavGraph> NavOctree = nullptr;
};
