// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"
#include "GameFramework/Actor.h"
#include "Actors/MercunaNavGraph.h"
#include "Objects/MercunaPath.h"

#include "Runtime/Launch/Resources/Version.h"

#include "MercunaNavTestingActor.generated.h"

UCLASS(Abstract, hidecategories = (Object, Actor, Input, Rendering, Blutilities), Blueprintable)
class MERCUNA_API AMercunaNavTestingActor : public AActor
{
    GENERATED_UCLASS_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercunaPathUpdated);

public:
    // Start search from this actor
    UPROPERTY(EditAnywhere, Category = Pathfinding)
    uint32 bSearchStart : 1;

    /** Determines what type of navigation modifier volumes the
    *   test paths are allowed to or are required to go through. */
    UPROPERTY(EditAnywhere, Category = Pathfinding)
    FMercunaPawnUsageFlags UsageFlags;

    // Maximum path length, used to limit pathfind.  0 means unlimited.
    UPROPERTY(EditAnywhere, Category = Pathfinding, AdvancedDisplay, meta = (ClampMin = "0.0"))
    float MaxPathLength = 0.0f;

    // Continuously repath, this can be useful for testing performance
    UPROPERTY(EditAnywhere, Category = Pathfinding, AdvancedDisplay)
    uint32 bContinuouslyRepath : 1;

    // Automatically repath on changes, level load, nav graph rebuild, etc
    UPROPERTY(EditAnywhere, Category = Pathfinding, AdvancedDisplay)
    bool bAutomaticRepath;

    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    FString PathStatus;

    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    uint32 bPathExist : 1;

    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    uint32 bPathIsPartial : 1;

    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    uint32 bPathIsOutOfNodes : 1;

    // The total length of line sections
    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    float PathLength;

    // Number of sections in the path
    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    int PathSections;

    // Time taken to do path finding search, in milliseconds.
    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    float PathFindTime;

    // Number of nodes added to the open list while performing the A* search to find the path
    UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category = PathfindingStatus)
    int PathNodesUsed;

    // Number of times to repeat the pathfind and display minimum time.
    UPROPERTY(EditAnywhere, Category = PathfindingStatus, AdvancedDisplay, meta = (ClampMin="1"))
    int PathFindRepeats = 1;

    // Use average time instead of minimum time when repeating pathfinds
    UPROPERTY(EditAnywhere, Category = PathfindingStatus, AdvancedDisplay)
    bool bDisplayAverageTime = false;

    //-------------------------------------------------------------------------

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
#else
    virtual void PreEditChange(FProperty* PropertyThatWillChange) override;
#endif
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    virtual void PostEditMove(bool bFinished) override;
    virtual void PostEditUndo() override;
    virtual void PostLoad() override;
#endif // WITH_EDITOR

    virtual void BeginDestroy() override;
    virtual void Destroyed() override;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool ShouldTickIfViewportsOnly() const override;

protected:
    // Sphere component
    UPROPERTY()
    TObjectPtr<class USphereComponent> SphereComponent;

    UPROPERTY(Transient)
    TObjectPtr<UMercunaPath> Path;

    UPROPERTY(Transient)
    TObjectPtr<AMercunaNavGraph> NavGraph;

    // Whether to trigger a path update on the next tick
    // Defaults to true so that the path always updates on the first tick
    bool bUpdatePathOnNextTick = true;

    // Marks whether a pathfind is in progress
    bool bRequestComplete;

    virtual AMercunaNavTestingActor* GetOtherActor() const { return nullptr; }
    virtual void SetOtherActor(AMercunaNavTestingActor* OtherActor) {}

    void UpdatePath();
    virtual void ClearPath();
    virtual void FindPath() {}
    virtual void BroadcastPathUpdated() {}

    UFUNCTION(meta = (CallInEditor = "true"))
    void OnNavGraphBuildComplete(bool Success);

    UFUNCTION(meta = (CallInEditor = "true"))
    void OnPathUpdate(EMercunaPathEvent::Type PathEvent);

    // Manually trigger a repath for a MercunaNavTestingActor.
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void Repath();

    // Perform a latent pathfind that triggers Completed once the pathfind is finished.
    // This is primarily intended for path testing. To use this function the Nav Testing
    // actor must have bAutomaticRepath disabled.
    UFUNCTION(BlueprintCallable, Category = "Mercuna", meta = (Latent, LatentInfo = "LatentInfo", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DevelopmentOnly))
    void PathfindLatent(bool& Result, UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, bool bAllowPartial = true);

private:
    void SetPathInfo(UMercunaPath* CurPath, EMercunaPathEvent::Type PathEvent);

    // Data for averaging time over multiple path finds
    int m_pathTrialCount;
    float m_pathTotalTime;
};
