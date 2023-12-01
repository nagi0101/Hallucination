// Copyright (C) 2018-2022 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Components/NavGraphRendering/MerNavGraphRenderingComponent.h"

#include "IMercunaEngine.h"

#include "MerNavOctreeRenderingComponent.generated.h"

namespace mercuna
{
    class IMerNavGraph3D;
    template<typename T> class MerPtr;
    struct MerNavOctreeDebugRenderComponentImpl;
}

UCLASS()
class UMerNavOctreeRenderingComponent : public UMerNavGraphRenderingComponent
{
    GENERATED_UCLASS_BODY()

public:
    ~UMerNavOctreeRenderingComponent();

    virtual void SetGraph(mercuna::MerPtr<mercuna::IMerNavGraph> pNavGraph) override;

    //~ Begin UActorComponent Interface.
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface.

    //~ Begin UPrimitiveComponent Interface.
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    //~ End UPrimitiveComponent Interface.

private:
    friend class FMerNavOctreeDebugRenderSceneProxy;

    // Read the colours from the editor settings and update the materials
    virtual void UpdateFromSettings() override;

    // Rebuild the octree drawing mesh
    void GenerateOctreeMesh(FMerNavGraphDebugRenderSceneProxyData& ProxyData);

    // Hide internal Mercuna types
    mercuna::MerNavOctreeDebugRenderComponentImpl* m_pImpl;
};
