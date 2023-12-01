// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "DebugRenderSceneProxy.h"
#include "Components/PrimitiveComponent.h"
#include "StaticMeshResources.h"
#include "DynamicMeshBuilder.h"

#include "MercunaTypes.h"

#include "MerNavGraphRenderingComponent.generated.h"

namespace mercuna
{
    class IMerNavGraph;
    template<typename T> class MerPtr;
}

struct FStaticMeshVertexBuffers;

//-----------------------------------------------------------------------------

class FMerNavGraphProxySection
{
public:
    UMaterialInterface* Material;

    FStaticMeshVertexBuffers VertexBuffers;
    FDynamicMeshIndexBuffer32 IndexBuffer;
    FLocalVertexFactory VertexFactory;
    EPrimitiveType PrimitiveType;

    FMatrix Transform;
    bool bShowBackFace;

    FMerNavGraphProxySection(ERHIFeatureLevel::Type InFeatureLevel, const FMatrix& Transform, EPrimitiveType PrimitiveType = PT_TriangleList, bool _bShowBackFace=false)
        : Material(nullptr)
        , VertexFactory(InFeatureLevel, "FMerNavMeshProxySection")
        , PrimitiveType(PrimitiveType)
        , Transform(Transform)
        , bShowBackFace(_bShowBackFace)
    {}

    void Init();
    void Release();
};

struct FMerNavGraphDebugRenderSceneProxyData
{
    ERHIFeatureLevel::Type InFeatureLevel;

    TArray<FMerNavGraphProxySection*> Sections;

    FMerNavGraphDebugRenderSceneProxyData(ERHIFeatureLevel::Type InFeatureLevel)
        : InFeatureLevel(InFeatureLevel) {}

    ~FMerNavGraphDebugRenderSceneProxyData()
    {
        for (FMerNavGraphProxySection* Section : Sections)
        {
            if (Section != nullptr)
            {
                Section->Release();
                delete Section;
            }
        }
    }

    FMerNavGraphProxySection* NewSection(const FMatrix& Transform, EPrimitiveType PrimitiveType = PT_TriangleList, bool bShowBackFace = false)
    {
        FMerNavGraphProxySection* Section = new FMerNavGraphProxySection(InFeatureLevel, Transform, PrimitiveType, bShowBackFace);
        Sections.Add(Section);
        return Section;
    }

    void Init()
    {
        for (FMerNavGraphProxySection* Section : Sections)
        {
            if (Section != nullptr)
            {
                Section->Init();
            }
        }
    }
};

//-----------------------------------------------------------------------------

UCLASS(Abstract, hideCategories = (Tags, Physics, Navigation, Mobile, Activation, Object, Replication, HLOD, Lighting, VirtualTexture, AssetUserData, Materials, MaterialParameters))
class UMerNavGraphRenderingComponent : public UPrimitiveComponent
{
    GENERATED_UCLASS_BODY()

public:
    virtual ~UMerNavGraphRenderingComponent() override;

    virtual void SetGraph(mercuna::MerPtr<mercuna::IMerNavGraph> pNavGraph);

    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void BeginDestroy() override;

    virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
    virtual void SetMaterial(int32 ElementIndex, class UMaterialInterface* Material) override;

    virtual void RenderDebugLabels(UCanvas* Canvas);

protected:
    // In World Partition levels should the currently loaded chunks bounds be drawn
    UPROPERTY(NonPIEDuplicateTransient, EditAnywhere, Category = "Debug Draw")
    bool bDrawChunkBounds = false;

    // Store the debug draw materials 
    UPROPERTY()
    TArray<UMaterialInterface*> Materials;

    TArray<FDebugRenderSceneProxy::FText3d> Labels;

    virtual void UpdateFromSettings() {}

    // Calculate the bounds from the sum of all the nav volumes currently associated with this graph
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

    void GenerateChunkBounds(FMerNavGraphProxySection* Section);
};
