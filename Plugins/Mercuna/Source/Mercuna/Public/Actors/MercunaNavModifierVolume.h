// Copyright (C) 2018-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "Runtime/Launch/Resources/Version.h"
#include "GameFramework/Actor.h"
#include "MercunaNavModifierVolume.generated.h"

class AMercunaNavGraph;

class UMerVolumeComponent;

//-----------------------------------------------------------------------------

/**
* Defines a volume in which pathfinding costs can be increased and pathfinding
* can be limited to specific navigation usage types.
*/
UCLASS(hidecategories=(Collision, Input, Physics, Networking))
class MERCUNA_API AMercunaNavModifierVolume : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    /** Resize the modifier volume at runtime.
    *   @param Size         New side length for each side of the box
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void SetSize(FVector Size);

    /** Get the unscaled size of the nav modifier volume */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    FVector GetSize() const;

    /** Enable or disable the modifier volume at runtime. 
    *   @param Enabled      Whether the modifier volume is now enabled.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void SetEnabled(bool Enabled);

    /** Set the usage type flags at runtime. 
    *   @param NewUsageTypes   The new usage types.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void SetUsageTypes(const FMercunaNavUsageTypes& NewUsageTypes);

    /** Set the cost multiplier on the modifier volume at runtime. 
    *   @param NewCostMultiplier  The updated cost multiplier.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void SetCostMultiplier(float NewCostMultiplier);

    /** Add the modifier volume to a nav graph at runtime.  The modifier
    *   volume must not already be associated with a nav graph.
    *   @param NavGraph  The navigation graph to add the modifier volume to.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void AddToGraph(AMercunaNavGraph* NavGraph);

    /** Remove the modifier volume from the nav graphs is it associated with at runtime.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void RemoveFromGraph();

    //-----------------------------------------------------------------------------

    bool IsEnabled() const { return bEnabled; }
    float GetCostModifier() const { return CostMultiplier; }
    FORCEINLINE const FMercunaNavUsageTypes& GetUsageTypes() const { return UsageTypes; }
    AMercunaNavGraph* GetNavGraph() const;

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
#else
    virtual void PreEditChange(FProperty* PropertyThatWillChange) override;
#endif
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    virtual void PostActorCreated() override;
    virtual void PostLoad() override;
    virtual void Destroyed() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual bool IsLevelBoundsRelevant() const override { return false; }

private:
    /** Which navigation graph is this modifier volume is associated with. */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    TSoftObjectPtr<AMercunaNavGraph> NavGraph;

    UPROPERTY(Category = "Mercuna", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMerVolumeComponent> VolumeComponent;

    /** If set, this modifier volume is currently enabled and its costs and 
    * usage flags will be included in pathfinds and other queries.
    */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    bool bEnabled;

    /** Amount by which to increase the cost of paths (multiplied by path distance) 
    * going through this modifier volume.
    */
    UPROPERTY(EditAnywhere, Category = "Mercuna", meta = (ClampMin = "1.0", ClampMax = "15.0"))
    float CostMultiplier;

    /** Set of flags that mark which usage types this modifier volume represents. 
    */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    FMercunaNavUsageTypes UsageTypes;

    // Was this modifier volume spawned, as opposed to loaded
    bool bSpawned = false;

    // Called when this actor is moved or modified
    void OnUpdated();

    // Following property only exists now for migration purposes
    UE_DEPRECATED(2.0, "Deprecated as no longer based on volume base class")
    UPROPERTY()
    TObjectPtr<class UBrushComponent> BrushComponent;
};
