// Copyright (C) 2020-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"
#include "UObject/ObjectMacros.h"
#include "Components/PrimitiveComponent.h"
#include "MerNavLinkRenderingComponent.generated.h"

UCLASS(hidecategories=Object, editinlinenew)
class MERCUNA_API UMerNavLinkRenderingComponent : public UPrimitiveComponent
{
    GENERATED_UCLASS_BODY()
        
    //~ Begin UPrimitiveComponent Interface
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

    /** Should recreate proxy on every update */
    virtual bool ShouldRecreateProxyOnUpdateTransform() const override { return true; }
#if WITH_EDITOR
    virtual bool ComponentIsTouchingSelectionBox(const FBox& InSelBBox, const FEngineShowFlags& ShowFlags, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;
    virtual bool ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const FEngineShowFlags& ShowFlags, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const override;
#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 22) || ENGINE_MAJOR_VERSION == 5
    virtual bool IgnoreBoundsForEditorFocus() const override { return true; }
#endif
#endif
    //~ End UPrimitiveComponent Interface

    //~ Begin USceneComponent Interface
    virtual FBoxSphereBounds CalcBounds(const FTransform &LocalToWorld) const override;
    //~ End USceneComponent Interface
};
