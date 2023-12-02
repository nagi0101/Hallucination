// Copyright (C) 2018-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "Runtime/Launch/Resources/Version.h"
#include "GameFramework/Actor.h"
#include "MercunaNavExclusionVolume.generated.h"

class AMercunaNavGraph;
class UMerVolumeComponent;
class UBrushComponent;

//-----------------------------------------------------------------------------

/**
* Defines a volume in which pathfinding is not allowed.
*/
UCLASS(hidecategories=(Collision))
class MERCUNA_API AMercunaNavExclusionVolume : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    /** Resize the nav exclusion volume at runtime.
    * @param Size  New side length for each side of the box
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void SetSize(FVector Size);

    /** Get the unscaled size of the nav exclusion volume */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    FVector GetSize() const;

    /** Add the nav exclusion volume to a nav graph at runtime.  The nav 
    *   exclusion volume must not already be associated with a nav graph. 
    *   @param NavGraph  The navigation graph to add the nav exclusion volume to.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void AddToGraph(AMercunaNavGraph* NavGraph);

    /** Add the nav exclusion volume to all nav graphs at runtime.  The nav
    *   exclusion volume must not already be associated with a nav graph.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void AddToAllGraphs();

    /** Remove the nav volume from all graphs is it associated with at runtime.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void RemoveFromGraphs();

    bool IsAssociated(AMercunaNavGraph* NavGraph) const;
    FORCEINLINE AMercunaNavGraph* GetNavGraph() const { return NavGraph; }
    FORCEINLINE bool AppliesToAllNavGraphs() const { return bApplyToAllNavGraphs; }

    virtual void PostActorCreated() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Destroyed() override;

    virtual void PostLoad() override;
    virtual bool IsLevelBoundsRelevant() const override { return true; }

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
#else
    virtual void PreEditChange(FProperty* PropertyThatWillChange) override;
#endif
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditMove(bool bFinished) override;
#endif

private:
    UPROPERTY(Category = Mercuna, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMerVolumeComponent> VolumeComponent;

    /** Whether this exclusion volume should apply to all navigation graphs. */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    bool bApplyToAllNavGraphs;

    /** Which navigation grid or octree this exclusion volume is associated with. */
    UPROPERTY(EditAnywhere, Category = "Mercuna", meta=(EditCondition="!bApplyToAllNavGraphs"))
    TObjectPtr<AMercunaNavGraph> NavGraph;

    // Was this exclusion volume spawned, as opposed to loaded
    bool bSpawned = false;

    // Called when this actor is moved or modified
    void OnUpdated();

    // Following property only exists now for migration purposes
    UE_DEPRECATED(2.0, "Deprecated as no longer based on volume base class")
    UPROPERTY()
    TObjectPtr<UBrushComponent> BrushComponent;
};
