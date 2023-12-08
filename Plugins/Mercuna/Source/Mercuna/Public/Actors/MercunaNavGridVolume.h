// Copyright (C) 2019-2022 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Actors/MercunaNavVolume.h"

#include "MercunaNavGridVolume.generated.h"

class AMercunaNavGraph2D;

/** Volume inside which nav grid will be generated to enable ground navigation */
UCLASS()
class MERCUNA_API AMercunaNavGridVolume : public AMercunaNavVolume
{
    GENERATED_UCLASS_BODY()

public:
    /** Add the nav volume to a grid at runtime.  The nav volume
    *   must not already be associated with a grid.
    *   @param NavGrid  The navigation grid to add the nav volume to.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void AddToGrid(AMercunaNavGraph2D* NavGrid);

    /** Remove the nav volume from an octree at runtime.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void RemoveFromGrid();

    // Get the nav graph this volume is linked to
    virtual AMercunaNavGraph* GetNavGraph() const override;
    AMercunaNavGraph2D* GetNavGrid() const;

    virtual void PostActorCreated() override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode)  override;

#if WITH_EDITOR
    virtual FName GetNavGraphPropertyName() const override;
#endif

protected:
    // The nav grid this volume belongs to
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    TSoftObjectPtr<AMercunaNavGraph2D> NavGrid;

    virtual void SetNavGraph(AMercunaNavGraph* NavGraph) override;
};
