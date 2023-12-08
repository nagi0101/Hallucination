// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"

#include "MercunaTypes.h"
#include "Actors/MercunaNavGraph.h"

#include "MercunaNavOctree.generated.h"

// Forward declarations
class AMercunaNavOctree;
class UMercunaPath;
class UMercunaSmoothPath;
class AMerNavDataChunk;

namespace mercuna
{
    class IMerNavGraph3D;
    struct MercunaNavOctreeImpl;
    template<typename T> class MerPtr;
    MERCUNA_API MerPtr<IMerNavGraph3D> GetMerNavGraph3D(AMercunaNavOctree* NavOctree);
    struct MerUsageSpec;
}

/** 3D navigation graph on which pawns with a Mercuna3DNavigationComponent can move */
UCLASS(hidecategories = (Input, Physics, Materials, Rendering, Tags, Actor, Layers, LOD, Cooking, Collision), NotBlueprintable)
class MERCUNA_API AMercunaNavOctree : public AMercunaNavGraph
{
    GENERATED_UCLASS_BODY()

public:
    ~AMercunaNavOctree();

    // Begin UObject Interface
    virtual void PostLoad() override;
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
    // End UObject Interface

    //-----------------------------------------------------------------------------
    // Generation parameters
    //-----------------------------------------------------------------------------

    /** The minimum pawn radius determine what navigation data is stored in the octree.
    * Paths will never go closer to geometry than the minimum pawn radius
    */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, meta = (ClampMin = "1", ClampMax = "6", ExposeOnSpawn = true))
    int32 MinPawnRadius;

    /** The maximum pawn radius determine what navigation data is stored in the octree.
    * The octree doesn't store data to allow paths to be found with more clearance from
    * geometry than the maximum radius.
    */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, meta = (ClampMin = "1", ClampMax = "29", ExposeOnSpawn = true))
    int32 MaxPawnRadius;

    /** Should navigation be restricted to MaxPawnRadius distances from surfaces. Any free
    * space further from geometry than this distance will be considered unnavigable. Useful
    * for pawns that should stay near geometry. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, AdvancedDisplay, meta = (ExposeOnSpawn = true))
    bool bRestrictToSurfaces;

    /** Whether to enable saving of octree deltas.  If set, then modifications to the octree
    * can be saved at runtime, for later reloading.  Note that this must not be set if
    * Allow Octree Merging is set. */
    UPROPERTY(EditAnywhere, Category = Runtime, config, AdvancedDisplay)
    bool bRecordOctreeDeltas;

    //-----------------------------------------------------------------------------

    /** Checks whether a position is in navigable space
    * @param Position           The position to clamp
    * @param NavigationRadius   The minimum size that navigable space must fit
    * @param Result             True if the position is in navigable space
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void IsNavigable(FVector Position, float NavigationRadius, bool& Result);

    /** Checks whether multiple positions are in navigable space
    * @param Positions          The positions to clamp
    * @param NavigationRadius   The minimum size that navigable space must fit
    * @param Results            True if a position is in navigable space
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void IsNavigableMulti(const TArray<FVector>& Positions, float NavigationRadius, TArray<bool>& Results);

    /** Clamp a position to navigable space.
    * @param Position           The position to clamp
    * @param NavigationRadius   The minimum size that navigable space must fit
    * @param SearchRadius       The maximum distance to search for a valid position
    * @param ClampedPosition    The position after it has been clamped
    * @param Result             True if the position has been successfully clamped to navigable space, false if no valid space within SearchRadius
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void ClampToNavigable(FVector Position, float NavigationRadius, float SearchRadius, FVector& ClampedPosition, bool& Result);

    /** Perform a raycast through the navigable octree. Result is true if the raycast
    *   succeeds, otherwise HitPosition is set to the position of the first hit
    * @param Start              The position to raycast from
    * @param End                The position to raycast to
    * @param NavigationRadius   The minimum size object that must be able to pass along the ray
    * @param HitPosition        The position that the ray hits
    * @param RayHit             True if the ray hits something, false if the ray is clear
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void Raycast(FVector Start, FVector End, float NavigationRadius, FVector& HitPosition, bool& RayHit);

    /** Check whether there is a path from Start to End. Returns true if path can be found
    * @param Start              The position to path from
    * @param End                The position to path to
    * @param NavigationRadius   The minimum size object that must be able to pass along the path
    * @param MaxPathLength      The maximum length the path is allowed to be
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    bool IsReachable(FVector Start, FVector End, float NavigationRadius, float MaxPathLength = 1000.f);

    /** Check whether there is a path from Start to each of the EndPos positions using a Latent action.
     * Calls back with the results, this call will be made on a worker thread.
     * @param Start             The position to path from
     * @param EndPos            The positions to path to
     * @param NavigationRadius   The minimum size object that must be able to pass along the path
     * @param MaxPathLength      The maximum length the path is allowed to be
     * @param AccuratePathLength If true, use a more expensive test that checks the path length accurately.
     *                           If false, use a cheap check test that may allow significantly longer paths.
     * @param StartSearchRadius  The Start position is clamped to navigable space by a search with radius StartSearchRadius
     *                           (a sensible default range is used if it is negative).
     * @param EndSearchRadius    The End positions are clamped to navigable space by a search with radius EndSearchRadius
     *                           (a sensible default range is used if it is negative).
     * @param Result             Output array of booleans indicating whether each position is reachable
     * @param ClampedPos         Output array of destination positions after they have been clamped to navigable space
     */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (Latent, LatentInfo = "LatentInfo", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    void IsReachableLatent(FVector Start, const TArray<FVector>& EndPos, float NavigationRadius,
                           TArray<bool>& Result, TArray<FVector>& ClampedPos, UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, 
                           float MaxPathLength = 1000.f, bool AccuratePathLength = true, float StartSearchRadius = -1.f, float EndSearchRadius = 0.f);

    /** Asynchronously check whether there is a path from Start to each the the EndPos positions.
     * Calls back with the results, this call will be made on a worker thread.
     * @param Start             The position to path from
     * @param EndPos            The positions to path to
     * @param NavigationRadius   The minimum size object that must be able to pass along the path
     * @param MaxPathLength      The maximum length the path is allowed to be
     * @param AccuratePathLength If true, use a more expensive test that checks the path length accurately.
     *                           If false, use a cheap check test that may allow significantly longer paths.
     * @param OnCompleteCallback Callback to make on completion, indicating whether the position is reachable,
     *                           the approximate path length to each end position, and the destination positions after clamping.
     *                           This callback is made on the game thread.
     * @param StartSearchRadius  The Start position is clamped to navigable space by a search with radius StartSearchRadius
     *                           (a sensible default range is used if it is negative).
     * @param EndSearchRadius    The End positions are clamped to navigable space by a search with radius EndSearchRadius
     *                           (a sensible default range is used if it is negative).
     * @param UsageFlags         Specifies the valid usage flags for the query
     */
    void IsReachableAsync(FVector Start, const TArray<FVector>& EndPos, float NavigationRadius, float MaxPathLength, bool AccuratePathLength,
                          TMercunaReachableCallback OnCompleteCallback, float StartSearchRadius = -1.f, float EndSearchRadius = 0.f, FMercunaPawnUsageFlags UsageSpec = FMercunaPawnUsageFlags());

    //-----------------------------------------------------------------------------

    /** Path find from Start to End, returning a MercunaPath object. The path find is performed
    *   asynchronously so the returned path is not immediately usable.
    * @param OnPathUpdated      Delegate that is called when path is generated or changes
    * @param Start              The position to path from
    * @param End                The position to path to
    * @param NavigationRadius   The minimum size object that must be able to pass along the path
    * @param AllowPartial       If the End point is not reachable is a partial path acceptable
    * @param MaxPathLength      Approximate maximum path distance to search, zero (the default) means no limit.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaPath* FindPathToLocation(const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, FVector End, float NavigationRadius, bool AllowPartial = false, float MaxPathLength = 0.f, float HeightChangePenalty = 0.f);
    UMercunaPath* FindPathToLocation(FVector Start, FVector End, float NavigationRadius, bool AllowPartial = false, float MaxPathLength = 0.f, float HeightChangePenalty = 0.f);

    /** Path find from Start through the given Destinations, returning a MercunaPath object. The path find is performed
    *   asynchronously so the returned path is not immediately usable.
    * @param OnPathUpdated      Delegate that is called when path is generated or changes
    * @param Start              The position to path from
    * @param Destinations       The positions to path through
    * @param NavigationRadius   The minimum size object that must be able to pass along the path
    * @param AllowPartial       If the End point is not reachable is a partial path acceptable
    * @param MaxPathLength      Approximate maximum path distance to search, zero (the default) means no limit.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaPath* FindPathToLocations(const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, const TArray<FVector>& Destinations, float NavigationRadius, bool AllowPartial = false, float MaxPathLength = 0.f, float HeightChangePenalty = 0.f);
    UMercunaPath* FindPathToLocations(FVector Start, const TArray<FVector>& Destinations, float NavigationRadius, bool AllowPartial = false, float MaxPathLength = 0.f, float HeightChangePenalty = 0.f);

    /** Path find from Start to Goal Actor, returning a MercunaPath object. The path find is performed
    *   asynchronously so the returned path is not immediately usable.
    * @param OnPathUpdated      Delegate that is called when path is generated or changes
    * @param Start              The position to path from
    * @param GoalActor          The actor to path to
    * @param NavigationRadius   The minimum size object that must be able to pass along the path
    * @param AllowPartial       If the goal actor is not reachable is a partial path acceptable
    * @param MaxPathLength      Approximate maximum path distance to search, zero (the default) means no limit.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaPath* FindPathToActor(const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, AActor* GoalActor, float NavigationRadius, bool AllowPartial = false, float MaxPathLength = 0.f, float HeightChangePenalty = 0.f);
    UMercunaPath* FindPathToActor(FVector Start, AActor* GoalActor, float NavigationRadius, bool AllowPartial = false, float MaxPathLength = 0.f, float HeightChangePenalty = 0.f);

    /** Find a spline from Start to End, returning a MercunaSpline object. The path find and spline
    *   construction is performed asynchronously so the returned spline is not immediately usable.
    * @param OnPathUpdated      Delegate that is called when path is generated or changes
    * @param Start              The position to path from
    * @param End                The position to path to
    * @param NavigationRadius   The minimum size object that must be able to pass along the path
    * @param MaxSpeed           The maximum speed of the actor what is following the spline
    * @param MaxAcceleration    The maximum acceleration of the actor following the spline
    * @param AllowPartial       If the End point is not reachable is a partial path acceptable
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaSmoothPath* FindSmoothPathToLocation(const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, FVector End, float NavigationRadius, float MaxSpeed, float MaxAcceleration, bool AllowPartial = false, float MaxPathLength = 0.f);
    UMercunaSmoothPath* FindSmoothPathToLocation(FVector Start, FVector End, float NavigationRadius, float MaxSpeed, float MaxAcceleration, bool AllowPartial = false, float MaxPathLength = 0.f);

    /** Find a spline from Start through the given Destinations, returning a MercunaSpline object. The path find and spline
    *   construction is performed asynchronously so the returned spline is not immediately usable.
    * @param OnPathUpdated      Delegate that is called when path is generated or changes
    * @param Start              The position to path from
    * @param Destinations       The positions to path through
    * @param NavigationRadius   The minimum size object that must be able to pass along the path
    * @param MaxSpeed           The maximum speed of the actor what is following the spline
    * @param MaxAcceleration    The maximum acceleration of the actor following the spline
    * @param AllowPartial       If the End point is not reachable is a partial path acceptable
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaSmoothPath* FindSmoothPathToLocations(const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, const TArray<FVector>& Destinations, float NavigationRadius, float MaxSpeed, float MaxAcceleration, bool AllowPartial = false, float MaxPathLength = 0.f);
    UMercunaSmoothPath* FindSmoothPathToLocations(FVector Start, const TArray<FVector>& Destinations, float NavigationRadius, float MaxSpeed, float MaxAcceleration, bool AllowPartial = false, float MaxPathLength = 0.f);

    //-----------------------------------------------------------------------------

#if WITH_EDITOR
    /** Load the octree from a file. Should only be used for debugging. Can't be used in PiE/SiE modes. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (DevelopmentOnly))
    bool LoadFromFile(FString Filename);
#endif

    //-----------------------------------------------------------------------------

    /** Save all Octree modifications made using RebuildVolume(s) to an archive. */
    void SaveDeltas(FArchive& Ar);

    /** Load Octree modifications from an archive.  This must be called immediately after serializing the octree -
    * before any other changes have been made.  Logs and returns false on failure. */
    bool LoadDeltas(FArchive& Ar);

    //-----------------------------------------------------------------------------

    /** Set the orientation of the octree and all associated volumes */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    virtual void SetNavigationRotation(const FRotator& Rotation) override;

    /** Build the octree. Should only be used at runtime.
    * @param PriorityPositions          Optional set of positions that the nav graph will be built outwards from
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (AutoCreateRefTerm = "PriorityPositions"))
    virtual void Build(const TArray<FVector>& PriorityPositions) override;

    /** Build the octree. Should only be used at runtime. */
    virtual void Build() override;

    /** Blueprint notification that low resolution octree is ready during a runtime build */
    UPROPERTY(BlueprintAssignable, Category = "Mercuna|3D Navigation")
    FMercunaBuildComplete OnBuildLowResReady;

    /** Rebuild a volume of the octree
    * @param Volume             An actor whose bounds specify the volume to rebuild
    * @param StagedBuild        Use when rebuilding a large volume to rebuild in two stages. First a low resolution octree is created to allow approximate navigation, then it is refined.
    * @param RemoveUnseeded     Whether to remove navigable space not reachable from any nav seed from the octree after rebuild
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void RebuildVolume(const AActor* Volume, bool StagedBuild = false, bool RemoveUnseeded = false);

    /** Rebuild multiple changed volumes, if the volumes are closer together this is more efficient than triggering many Rebuilds.
    * @param Volumes       An array of FBox's bounding the volume to rebuild.
    * @param StagedBuild   Use when rebuilding a large volume to rebuild in two stages. First a low resolution octree is created to allow approximate navigation, then it is refined.
    * @param RemoveUnseeded     Whether to remove navigable space not reachable from any nav seed from the octree after rebuild
    * @param bMergeVolumes      Should nearby volumes should be merged together. Regenerating fewer large volumes can be more efficient than lots of individual smaller volumes
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void RebuildVolumes(const TArray<FBox>& Volumes, bool bStagedBuild = false, bool bRemoveUnseeded = false, bool bMergeVolumes = false);

    /** Rebuild the nav octree to reflect changes due to spawned, destroyed or moved Nav Volumes and Nav Exclusion Volumes.
    * @param StagedBuild   Use when rebuilding a large volume to rebuild in two stages. First a low resolution octree is created to allow approximate navigation, then it is refined.
    * @param RemoveUnseeded     Whether to remove navigable space not reachable from any nav seed from the octree after rebuild
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void RebuildChanges(bool StagedBuild = false, bool RemoveUnseeded = false);

    // Get whether the graph currently building
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    bool IsBuilding() const override;

    //-----------------------------------------------------------------------------

    /** Blueprint notification that a build has completed */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|3D Navigation")
    FMercunaBuildComplete OnBuildComplete;

    /** Blueprint notification that rebuilding a volume has completed*/
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|3D Navigation")
    FMercunaRebuildComplete OnRebuildComplete;

    /** Blueprint notification that low resolution octree is ready for a volume being rebuilt */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|3D Navigation")
    FMercunaRebuildComplete OnRebuildLowResReady;

    /** Blueprint notification that the octree is completely loaded.
     * This normally happens immediately on level load, but if the octree is loaded by level streaming and is
     * merged with another octree this event fires after the octrees are completely merged. */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|3D Navigation")
    FMercunaLoadComplete OnLoadComplete;

    /** Blueprint notification that the short range navigation data for this octree is loaded.
     * Navigation using this octree is not possible until this event fires.
     * This normally happens immediately on level load, but if the octree is loaded by level streaming and is
     * merged with another octree this event fires at the point pathfinding will work for short range paths.
     * Hierarchical pathfinding between the levels will be available once OnLoadComplete fires. */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|3D Navigation")
    FMercunaLoadComplete OnShortRangeLoadComplete;

    /** Blueprint notification that a new chunk of grid has been added.
     * This happens in World Partition maps when a new part of the map is loaded and merged in. */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|3D Navigation")
    FMercunaChunkAdded OnChunkAdded;

    //-----------------------------------------------------------------------------

#if WITH_EDITOR
    // Trigger asynchronous building of the octree. Can only be called while in edit mode.
    // Use Build for runtime building
    virtual void EditorBuild() override;
    virtual void EditorBuild(const FBox& WorldPartitionBounds) override;
#endif

    // Get/Set the debug draw mode of the octree.
    FORCEINLINE EMerOctreeDebugDrawMode GetDebugDrawMode() const { return DebugDrawMode; }
    void SetDebugDrawMode(EMerOctreeDebugDrawMode mode);
#if WITH_EDITORONLY_DATA
    void SetSelectedSeed(class AMercunaNavSeed* seed);
#endif

    // Internal versions of path find to explicitly request debug draw
    UMercunaPath* FindPathToActorWithDebug(FVector Start, AActor* GoalActor, float NavigationRadius, bool AllowPartial, float MaxPathLength, float HeightChangePenalty, bool SetPathfindDebugOnOctree, const mercuna::MerUsageSpec& usageSpec, const FOnMercunaPathUpdated& OnPathUpdatedDelegate = FOnMercunaPathUpdated());
    UMercunaPath* FindPathToLocationWithDebug(FVector Start, FVector End, const TArray<FVector>& Waypoints, float NavigationRadius, bool AllowPartial, float MaxPathLength, float HeightChangePenalty, bool SetPathfindDebugOnOctree, const mercuna::MerUsageSpec& usageSpec, const FOnMercunaPathUpdated& OnPathUpdatedDelegate = FOnMercunaPathUpdated());
    UMercunaSmoothPath* FindSmoothPathToLocationWithDebug(FVector Start, FVector End, const TArray<FVector>& Waypoints, float NavigationRadius, float MaxSpeed, float MaxAcceleration, bool AllowPartial, float MaxPathLength, float HeightChangePenalty, bool SetPathfindDebugOnOctree, const mercuna::MerUsageSpec& usageSpec, const FOnMercunaPathUpdated& OnPathUpdatedDelegate = FOnMercunaPathUpdated());

    //-----------------------------------------------------------------------------

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Serialize(FArchive& Ar) override;

    //-----------------------------------------------------------------------------

    // Returns the first instance found of the NavOctree in the given World and Level
    static AMercunaNavOctree* GetInstance(UWorld* World, ULevel* Level);

    // Get the most suitable octree
    static AMercunaNavOctree* GetBestInstance(UWorld* World, FVector Position, float NavigationRadius);

    // Number of octrees that exist in this world
    static size_t GetNumInstances(UWorld* World);

    // Number of octrees currently being built
    static size_t GetNumBuilding(UWorld* World);

    // Create a new NavOctree in the specified world and level
    static AMercunaNavOctree* CreateInstance(UWorld* World, ULevel* Level);

    virtual FBox GetNavVolumeBounds() const override;

    void DebugDraw();

private:
    friend AMerNavDataChunk;
    friend mercuna::MerPtr<mercuna::IMerNavGraph3D> mercuna::GetMerNavGraph3D(AMercunaNavOctree* NavOctree);

    // Returns the internal impl that contains Mercuna types
    mercuna::MercunaNavOctreeImpl* GetImpl() const;

    // Set up an empty graph
    virtual void SetupEmpty() override;

    // Get an octree with matching settings to this octree.
    AMercunaNavGraph* GetMatchingNavGraph() const override;

    // Trigger a merge of this octree into another octree
    void DoMerge(AMercunaNavGraph* pOtherNavGraph) override;

#if WITH_EDITOR
    // Calculate how far extra geometry needs to be collected so that the edges
    // of the graph are correct when only building part of the graph
    virtual FVector CalculateRequiredBuildPadding() const override;
#endif

    // Merge a chunk of graph into this graph
    virtual bool AddNavDataChunk(AMerNavDataChunk* DataChunk) override;

    // Serialize section of graph that overlaps the specified bounds
    virtual void SavePart(FArchive& Ar, const FBox& Bounds, TMap<TObjectPtr<const UPhysicalMaterial>, uint32>& PartSurfaceMaterials) const override;

    // Current debug draw mode
    UPROPERTY(Transient)
    EMerOctreeDebugDrawMode DebugDrawMode;

#if WITH_EDITORONLY_DATA
    // In editor if a nav seed is selected and seeded debug draw mode is active, 
    // just display all the cells in the seeds regions
    UPROPERTY(Transient)
    TObjectPtr<class AMercunaNavSeed> DebugDrawSeed;
#endif
};
