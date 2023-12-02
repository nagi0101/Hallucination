// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Actors/MercunaNavVolume.h"

#include "MercunaNavOctreeVolume.generated.h"

class AMercunaNavOctree;

/** Volume inside which nav octree will be generated to enable 3D navigation */
UCLASS()
class MERCUNA_API AMercunaNavOctreeVolume : public AMercunaNavVolume
{
    GENERATED_UCLASS_BODY()

public:
    /** Add the nav volume to an octree at runtime.  The nav volume
    *   must not already be associated with an octree.
    *   @param NavOctree  The navigation octree to add the nav volume to.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void AddToOctree(AMercunaNavOctree* NavOctree);

    /** Remove the nav volume from an octree at runtime.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void RemoveFromOctree();

    // Get the nav graph this volume is linked to
    virtual AMercunaNavGraph* GetNavGraph() const override;
    AMercunaNavOctree* GetNavOctree() const;

    FORCEINLINE unsigned int GetLOD() const { return (unsigned int)LOD; }
    FORCEINLINE bool GetFullDetailBuild() const { return FullDetailBuild; }

    virtual void PostActorCreated() override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode)  override;

#if WITH_EDITOR
    virtual FName GetNavGraphPropertyName() const override;
#endif


protected:
    // The 3D nav graph this volume belongs to
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    TSoftObjectPtr<AMercunaNavOctree> NavOctree;

    /** Level of detail to store for this nav volume. */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    EMercunaLevelOfDetail LOD = EMercunaLevelOfDetail::Full;

    /** Whether to build the nav volume at full detail if the LOD to store is not Full.  
     * Setting this increases build time a little but gives more accurate results when 
     * LOD is not set to Full. */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    bool FullDetailBuild = true;

    virtual void SetNavGraph(AMercunaNavGraph* NavGraph) override;
};
