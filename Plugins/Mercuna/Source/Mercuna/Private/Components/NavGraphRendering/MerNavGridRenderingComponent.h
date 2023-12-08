// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Components/NavGraphRendering/MerNavGraphRenderingComponent.h"

#include "IMercunaEngine.h"

#include "MerNavGridRenderingComponent.generated.h"

namespace mercuna
{
    class IMerNavGrid;
    template<typename T> class MerPtr;
    struct MerNavGridDebugRenderComponentImpl;
}

struct FMerNavGridDebugRenderSceneProxyData;

UCLASS()
class UMerNavGridRenderingComponent : public UMerNavGraphRenderingComponent
{
    GENERATED_UCLASS_BODY()

public:
    ~UMerNavGridRenderingComponent();

    virtual void SetGraph(mercuna::MerPtr<mercuna::IMerNavGraph> pNavGraph) override;

    // Write a copy of the mesh data to an archive. Useful for getting debug draw
    // on a server. Clears the dirty flag in the process.
    // The ViewPos is not currently used for query debug draw
    void GetMeshData(FArchive& Ar, const FVector& ViewPos);

    // Read mesh data from an archive. Useful for setting the debug draw on a client. This
    // data will be cached and then in preference to any data from the grid
    void SetMeshData(FArchive& Ar);

    // Clears any cached mesh data
    void ClearMeshData();

    // Has the debug draw changed (either due to a grid change or new query data) since
    // it was last rendered or copied
    bool IsDebugDrawDirty() const;

    //~ Begin UActorComponent Interface.
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface.

    //~ Begin UPrimitiveComponent Interface
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    //~ End UPrimitiveComponent Interface

private:
    friend class FMerNavGridDebugRenderSceneProxy;

    // When drawing the polymesh should the individual polygons be drawn
    UPROPERTY(NonPIEDuplicateTransient, EditAnywhere, Category = "Debug Draw")
    bool bDrawPolygons = true;

    // Draw all built generation boxes
    UPROPERTY(NonPIEDuplicateTransient, EditAnywhere, Category = "Debug Draw")
    bool bDrawGenerationBoxes = false;

    // Draw auto-generated jump links
    UPROPERTY(NonPIEDuplicateTransient, EditAnywhere, Category = "Debug Draw")
    bool bDrawAutoJumpLinks = true;

    // Hide internal Mercuna types
    mercuna::MerNavGridDebugRenderComponentImpl* m_pImpl;

    // How far off the geometry to draw the grid. Comes from settings
    float VerticalOffset = 0.0f;
    
    // Read the debug configuration from the editor settings and update the materials
    virtual void UpdateFromSettings() override;

    void GenerateData(FMerNavGridDebugRenderSceneProxyData& ProxyData);

    void GenerateGenerationBoxes(FMerNavGraphProxySection* Section);
};
