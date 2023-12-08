// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"
#include "GameFramework/Actor.h"
#include "Engine/LatentActionManager.h"
#include "WorldPartition/WorldPartitionHandle.h"

#include "MercunaNavGraph.generated.h"

// Forward declarations
class UPhysicalMaterial;
class FLoaderAdapterShape;

class AMercunaNavVolume;
class AMercunaNavExclusionVolume;
class AMercunaNavModifierVolume;
class AMerNavDataChunk;
class UMerNavGraphRenderingComponent;
class AMercunaNavGraph;

namespace mercuna
{
    struct MercunaNavGraphImpl;
    class IMerNavGraph;
    template<typename T> class MerPtr;
    MERCUNA_API MerPtr<IMerNavGraph> GetMerNavGraph(AMercunaNavGraph* NavGraph);
    MERCUNA_API MerPtr<IMerNavGraph> GetMerNavGraphIfMaster(AMercunaNavGraph* NavGraph);
}

// Result from an IsReachable query against a nav graph.
struct FMercunaReachableResult
{
    bool IsReachable;
    float ApproxPathDistance;
    FVector ClampedEndPos;
};

//-----------------------------------------------------------------------------
// Abstract base class for Mercuna navigation graphs
//-----------------------------------------------------------------------------

UCLASS(Abstract)
class MERCUNA_API AMercunaNavGraph : public AActor
{
    GENERATED_UCLASS_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercunaBuildComplete, bool, Success);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMercunaRebuildComplete, const AActor*, Volume, bool, Success);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercunaLoadComplete);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercunaChunkAdded, FBox, Bounds);

    typedef TFunction<void(const TArray<FMercunaReachableResult>&)> TMercunaReachableCallback;

public:
    //-----------------------------------------------------------------------------
    // Generation parameters
    //-----------------------------------------------------------------------------

    /** Determines the side length of the cells that make up the lowest level of the graph.
    * Cells are considered unnavigable if there is any level geometry within them, so the
    * larger the cell size the greater the error margin in the representation of the geometry.
    */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, meta = (ClampMin = "1.0", ExposeOnSpawn = true))
    float CellSize;

    /** Only actors that have a collision response that blocks this collision channel
    * (and have Can Ever Effect Navigation set) will be included in the graph. */
    UPROPERTY(EditAnywhere, Category = Generation, config, AdvancedDisplay)
    TEnumAsByte<ECollisionChannel> GeometryCollisionChannel;

    //-----------------------------------------------------------------------------

    /** Prevent the graph data from being saved. Used when the graph will be completely
    * generated at runtime.
    */
    UPROPERTY(EditAnywhere, Category = Runtime, config)
    bool bNeverSave;

    /** If set, then when a level is loaded at runtime, this nav graph can merge with another nav graphs
     * to allow continuous navigation between the nav graphs. Merging can only happen when both nav graphs
     * are set to allow merging and have the same configuration parameters. */
    UPROPERTY(EditAnywhere, Category = Runtime, config)
    bool bAllowNavGraphMerging;
    
    /** Controls where navigation data is stored when World Partition is enabled.
     * If set, then all nav graph data is saved in the nav graph actor that is always loaded.
     * If false, then data is stored in chunks that are spatially loaded by the World Partition system. */
    UPROPERTY(EditAnywhere, Category = Runtime, AdvancedDisplay, config)
    bool bDisablePartitionOfNavData;

    //-----------------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void PostActorCreated() override;
    virtual void PostLoad() override;
    virtual void PreInitializeComponents() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void BeginDestroy() override;
    virtual void Destroyed() override;
    virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditMove(bool bFinished) override;
    virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
#endif // WITH_EDITOR

    virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;

    // Tick even in Editor
    virtual void Tick(float DeltaSeconds) override;
    virtual bool ShouldTickIfViewportsOnly() const override { return true; }

    // Used to debug draw text
    void RenderDebugLabels(UCanvas* Canvas);

    // Reset the graph, stopping any in progress builds and deleting the internal graph
    void Clear();

    // Get the world velocity of a position within the graph
    FVector GetRelativeVelocity(const FVector& Position) const;

    //-----------------------------------------------------------------------------

#if WITH_EDITOR
    // Trigger asynchronous building. Can only be called while in edit mode.
    // Use Build for runtime building
    virtual void EditorBuild() {}
    virtual void EditorBuild(const FBox& WorldPartitionBounds) {}
#endif

    /** Trigger a build. Should only be used at runtime. */
    virtual void Build() {}

    /** Trigger a build. Should only be used at runtime.
     * @param PriorityPositions          Optional set of positions that the nav graph will be built outwards from
     */
    virtual void Build(const TArray<FVector>& PriorityPositions) {}

    // Note that something has changed in the level, so that the graph should be rebuilt
    void MarkForRebuild();

    // Check whether the graph needs to be rebuilt
    virtual bool NeedsRebuild() const { return !bNeverSave && bNeedsRebuild; }

    // Check whether the nav graph is currently building.  This only returns true when a build is scheduled
    // by the user of this API, not for internally triggered builds (e.g. nav invokers), or other work
    // such as merging of nav graphs due to level streaming.
    virtual bool IsBuilding() const;

    // Check whether any work that changes the nav graph is in progress.  This includes normal builds and
    // rebuilds, updates to modifier volumes, builds due to nav invokers, and merging/purging of the
    // nav graph due to level streaming.
    virtual bool IsAnyWorkInProgress() const;

    // Whether the last build succeed
    bool DidBuildSucceed() const;

    // If the last build failed what was the error
    FString GetBuildError() const;

    // Get generation progress
    float GetBuildProgress() const;

    // Get the time the last build was started
    double GetLastEditorBuildStartTime() const;

    //-----------------------------------------------------------------------------

    // Test whether an actor would be included in this nav graph
    virtual bool IsNavigationRelevant(const AActor* Actor) const;

    // Test whether a component would be included in this nav graph
    virtual bool IsNavigationRelevant(const UActorComponent* Component) const;

    //-------------------------------------------------------------------------

    /** Set the orientation of the nav graph and all associated volumes */
    virtual void SetNavigationRotation(const FRotator& Rotation) {}
    virtual void SetNavigationOrientation(const FQuat& Orientation);

    // Whether to allow modifier volumes to be rotated
    virtual bool AllowModifierVolumeRotation() const { return false; }

    //-------------------------------------------------------------------------

    virtual uint32 GetOrAddMaterialId(const UPhysicalMaterial* Material) { return 0; }

    //-------------------------------------------------------------------------

    // Whether to cut stationary obstacles out from the nav graph (currently only supported by nav grids).
    virtual bool CutOutStationaryObstacles() const { return false; }

    //-----------------------------------------------------------------------------
    //
    // These are used internally by the volume actors, and shouldn't be called directly!
    //

    // Changes to nav and exclusion volumes only happen once EditorBuild, Build, or RebuildChanges has been called

    /** Add a new nav volume to the graph */
    void AddNavVolume(AMercunaNavVolume* NavVolume, bool bAddImmediately);

    /** Remove an existing nav volume from the graph. */
    void RemoveNavVolume(AMercunaNavVolume* NavVolume, bool bRemoveImmediately);

    /** Move/modify a nav volume at runtime. */
    void UpdateNavVolume(AMercunaNavVolume* NavVolume);

    // The relative box that encompasses all the nav bounds associated with this graph
    virtual FBox GetNavVolumeBounds() const { return FBox(); }

    /** Add a new exclusion volume to the graph at runtime. */
    void AddExclusionVolume(AMercunaNavExclusionVolume* ExclusionVolume, bool bVolumeNeedsRebuild);

    /** Move/modify a exclusion volume at runtime. */
    void UpdateExclusionVolume(AMercunaNavExclusionVolume* ExclusionVolume);

    /** Remove an existing exclusion volume from the graph at runtime. */
    void RemoveExclusionVolume(AMercunaNavExclusionVolume* ExclusionVolume, bool bVolumeNeedsRebuild);

    // Changes to modifier volumes happen immediately

    /** Add a new modifier volume to the graph at runtime. */
    void AddModifierVolume(AMercunaNavModifierVolume* ModifierVolume, bool bVolumeNeedsRebuild);

    /** Update a modifier volume at runtime. */
    void UpdateModifierVolume(AMercunaNavModifierVolume* ModifierVolume);

    /** Remove an existing modifier volume from the graph at runtime. */
    void RemoveModifierVolume(AMercunaNavModifierVolume* ModifierVolume, bool bVolumeNeedsRebuild);

    //-------------------------------------------------------------------------

    // Get the name of this nav graph including its package name 
    FString GetFullName() const;

    //-------------------------------------------------------------------------

    // Get a nav graph in the level, returns null if there are suitable
    // graphs of different types present in the level.
    static AMercunaNavGraph* GetInstance(UWorld* World, ULevel* Level);

    // Get the master nav graph for this nav graph, that is the nav graph that this one has been
    // merged into if level streaming is being used.
    AMercunaNavGraph* GetMaster();

    /** Save the nav graph to the specified file. Overwrites the file if it already exists. Should only be used for debugging */
    UFUNCTION(BlueprintCallable, Category = "Mercuna", meta = (DevelopmentOnly))
    void SaveToFile(FString Filename) const;

    /** Write the graph to an archive.
     * If the never save property is set or a build is in progress then the save will fail, unless force save is set.
     * If a build is in progress and force save is used, the data might not be consistent.  If you are using the saved data
     * for any purposes other than debug, you should ensure IsBuilding() returns false before calling Save with force save true. */
    void Save(FArchive& Ar, bool bForceSave = false) const;

    /** Read the graph from an archive.  After calling this function you must call PostLoad() to complete the load. */
    bool Load(FArchive& Ar);

    // Get all the nav data chunk (as actors) that are present in this world
    static TArray<AActor*> GetNavDataChunkActors(const UWorld* World);

protected:
    friend mercuna::MerPtr<mercuna::IMerNavGraph> mercuna::GetMerNavGraph(AMercunaNavGraph* NavGraph);
    friend mercuna::MerPtr<mercuna::IMerNavGraph> mercuna::GetMerNavGraphIfMaster(AMercunaNavGraph* NavGraph);
    friend AMerNavDataChunk;
    friend UMerNavGraphRenderingComponent;

    // Hides internal Mercuna types
    friend mercuna::MercunaNavGraphImpl;
    mercuna::MercunaNavGraphImpl* m_pImpl;

    // Was this graph build when the map was loaded as a sublevel
    UPROPERTY()
    bool bBuildAsSublevel;

    // Was this graph build in a map that is using world partitioning (with streaming on)
    UPROPERTY()
    bool bUsingChunks;

    // Has there been a change in the map and does the graph need to be rebuilt
    UPROPERTY()
    bool bNeedsRebuild;

    // Game time that the graph was last build was started. Will be zero if the octree has 
    // not been rebuild since the level loaded
    double LastEditorBuildStartTime;

    // The set of nav volumes currently registered with this graph
    TSet<TWeakObjectPtr<AMercunaNavVolume>> NavVolumes;

    // A store of the bounds of all the chunks currently loaded
    TMap<FIntVector, FBox> LoadedChunkBounds;

    // Remember the position and orientation in order to work out graph velocity
    FVector OldPosition;
    FQuat OldOrientation;

    // Track the angular velocity needed if moving
    FVector AngularVelocity;

    // Component for debug drawing the graph
    UPROPERTY(VisibleAnywhere, Transient, Category = "Debug Draw")
    TObjectPtr<class UMerNavGraphRenderingComponent> MerNavGraphRenderingComponent;

#if WITH_EDITOR
    // Queue an editor build to be run on the main thread
    void RequestEditorBuild();
#endif

    //  Queue a runtime build to be run on the main thread
    void RequestBuild();

    // Callback for when the level containing this graph is moved in the editor
    void OnLevelTransform(const FTransform& transform);

    // Set up an empty graph
    virtual void SetupEmpty();

    // Gather and set the nav volumes (including exclusion volumes) on the internal nav graph
    bool SetupNavVolumes(bool bAllowZeroNavVolumes);

    // Get a nav graph with matching settings to this nav graph.
    virtual AMercunaNavGraph* GetMatchingNavGraph() const;

    // Trigger a merge of this nav graph into another nav graph
    virtual void DoMerge(AMercunaNavGraph* pOtherNavGraph);

    // Transfer master from another nav graph on unload of the old master
    virtual void TransferMaster(AMercunaNavGraph* pFromNavGraph) {}

    // Trigger a rebuild of changed volumes, if required
    virtual void TriggerRebuildChanges() {}

#if WITH_EDITOR
    // Calculate how far extra geometry needs to be collected so that the edges
    // of the graph are correct when only building part of the graph
    virtual FVector CalculateRequiredBuildPadding() const { return FVector(0); }

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
    // Spawn nav data chunks for an currently loaded editor regions
    void CreateNavDataChunks(const FVector& ChunkSize, const FBox& Bounds, TArray<FLoaderAdapterShape*>& LoadedAdapters, TArray<FBox>& EditorCellVolumes);
#else
    // Spawn nav data chunks for an currently loaded editor cells
    void CreateNavDataChunks(const FVector& AlignTo, const FBox& Bounds, TArray<FWorldPartitionReference>& LoadedActors, TArray<FBox>& EditorCellVolumes);
#endif

    // Destroy any nav data chunks associated with this nav graph (or that have been orphaned)
    void DestroyNavDataChunks(const FBox& Bounds);

    // Mark all nav data chunks associated with this nav graph as dirty.
    void MarkChunksDirty();

    // Check whether the graph will be generated in chunks
    bool WillUseChunks() const;
#endif

    // Merge the data from a chunk into this graph. Returns true if the chunk is successfully added
    virtual bool AddNavDataChunk(AMerNavDataChunk* DataChunk) { return false; }

    // Remove the part of the graph that correspond to this chunk
    void RemoveNavDataChunk(const AMerNavDataChunk* DataChunk);

    // Serialize section of graph that overlaps the specified bounds
    virtual void SavePart(FArchive& Ar, const FBox& Bounds, TMap<TObjectPtr<const UPhysicalMaterial>, uint32>& PartSurfaceMaterials) const {}

    // Temporarily serialize section of graph that overlaps the specified bounds.  This temporary
    // data can only be reloaded to the same instance of the nav graph.
    virtual void SavePartTemp(AMerNavDataChunk* DataChunk);

    // Whether nav volumes should always be added/removed from the underlying internal nav graph immediately,
    // instead of waiting for the next build.
    virtual bool AlwaysModifyNavVolumesImmediately() const { return false; }
};
