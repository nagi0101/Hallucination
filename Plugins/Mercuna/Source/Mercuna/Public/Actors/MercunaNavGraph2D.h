// Copyright (C) 2021-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Actors/MercunaNavGraph.h"

#include "MercunaTypes.h"

#include "MercunaNavGraph2D.generated.h"

// Forward declarations
class AMercunaNavLink;
class AMercunaNavGraph2D;

namespace mercuna
{
    class IMerNavGraph2D;
    struct MercunaNavGraph2DImpl;
    template<typename T> class MerPtr;
    MERCUNA_API MerPtr<IMerNavGraph2D> GetMerNavGraph2D(AMercunaNavGraph2D* NavGraph2D);
}

//-----------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct MERCUNA_API FMercunaSupportedAgentTypes
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, Category = AgentType)
    TMap<FMercunaAgentType, int> Types;

    UPROPERTY()
    TArray<FMercunaAgentType> IndexToType;

    UPROPERTY()
    EMercunaGridType GridType = EMercunaGridType::Ground;
};


inline bool operator==(const FMercunaSupportedAgentTypes& a, const FMercunaSupportedAgentTypes& b)
{
    if (a.GridType != b.GridType || a.Types.Num() != b.Types.Num()) return false;

    for (auto It : a.Types)
    {
        if (!b.Types.Contains(It.Key)) return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Abstract base class for Mercuna 2D navigation graphs
//-----------------------------------------------------------------------------

UCLASS(Abstract)
class MERCUNA_API AMercunaNavGraph2D : public AMercunaNavGraph
{
    GENERATED_UCLASS_BODY()

public:
    //-----------------------------------------------------------------------------

    /** Whether physical materials should be considered when generating the nav grid. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, AdvancedDisplay)
    bool bUsePhysicalMaterials;

    /** Set the priority for physical materials in the case of overlapping surfaces.
    * Physical materials at the start of the array will be considered higher priority */
    UPROPERTY(EditAnywhere, Category = Generation, AdvancedDisplay)
    TArray<TObjectPtr<const UPhysicalMaterial>> PhysicalMaterialPriorities;

    //-----------------------------------------------------------------------------

    /** Blueprint notification that a build has completed */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|Ground Navigation")
    FMercunaBuildComplete OnBuildComplete;

    /** Blueprint notification that rebuilding a volume has completed*/
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|Ground Navigation")
    FMercunaRebuildComplete OnRebuildComplete;

#if WITH_EDITOR
    // Trigger asynchronous building of the grid. Can only be called while in edit mode.
    // Use Build for runtime building
    virtual void EditorBuild() override;
    virtual void EditorBuild(const FBox& WorldPartitionBounds) override;
#endif

    /** Build the grid. Should only be used at runtime.
    * @param PriorityPositions          Optional set of positions that the nav graph will be built outwards from
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AutoCreateRefTerm = "PriorityPositions"))
    virtual void Build(const TArray<FVector>& PriorityPositions) override;

    /** Build the grid. Should only be used at runtime. */
    virtual void Build() override;

    /** Rebuild a volume of the grid
    * @param Volume             An actor whose bounds specify the volume to rebuild
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void RebuildVolume(const AActor* Volume);

    /** Rebuild multiple changed volumes, if the volumes are closer together this is more efficient than triggering many Rebuilds.
    * @param Volumes            An array of FBox's bounding the volume to rebuild.
    * @param bMergeVolumes      Should nearby volumes should be merged together. Regenerating fewer large volumes can be more efficient than lots of individual smaller volumes
    * @param bOnlyUnbuilt       Only build the part of the volume not already present in the nav grid.  This can be used to extend the built area
    *                           when a nav grid has been Created without building the whole of it.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta=(AdvancedDisplay = "2"))
    void RebuildVolumes(const TArray<FBox>& Volumes, bool bMergeVolumes = false, bool bOnlyUnbuilt = false);

    /** Rebuild the nav graph to reflect changes due to spawned, destroyed or moved Nav Volumes and Nav Exclusion Volumes.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void RebuildChanges();

    // Check whether the nav graph is currently building.  This only returns true when a build is scheduled
    // by the user of this API, not for internally triggered builds (e.g. nav invokers), or other work
    // such as merging of nav graphs due to level streaming.
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    virtual bool IsBuilding() const override;

    // Check whether any work that changes the nav graph is in progress.  This includes normal builds and
    // rebuilds, updates to modifier volumes, builds due to nav invokers, and merging/purging of the
    // nav graph due to level streaming.
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    virtual bool IsAnyWorkInProgress() const override;

    // Get the number of generation boxes currently building or queued for building.  This includes boxes
    // being generated due to nav invokers.
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    virtual int GetNumBoxesBuilding() const; 

    //-----------------------------------------------------------------------------

    /** Blueprint notification that the grid is completely loaded.
     * This normally happens immediately on level load, but if the grid is loaded by level streaming and is
     * merged with another grid this event fires after the grids are completely merged. */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|Ground Navigation")
    FMercunaLoadComplete OnLoadComplete;

    /** Blueprint notification that a new chunk of grid has been added.
     * This happens in World Partition maps when a new part of the map is loaded and merged in. */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna|Ground Navigation")
    FMercunaChunkAdded OnChunkAdded;

    //-----------------------------------------------------------------------------

    // Does this graph support this agent type
    bool IsAgentTypeSupported(const FMercunaAgentType& AgentType) const;
    int GetAgentTypeIdx(const FMercunaAgentType& AgentType) const;

    // Get all the agent types this graph supports
    TArray<FMercunaAgentType> GetSupportedAgentTypes() const;

    //-----------------------------------------------------------------------------

    // Number of 2D graphs that exist in this world
    static size_t GetNumInstances(UWorld* World);

    // Returns the first instance found of the NavGrid in the given World and Level
    static AMercunaNavGraph2D* GetInstance(UWorld* World, ULevel* Level);

    // Number of NavGrids currently being built
    static size_t GetNumBuilding(UWorld* World);

    // Get the most suitable grid
    static AMercunaNavGraph2D* GetBestInstance(UWorld* World, const FVector& Position, const FMercunaAgentType& AgentType);

    //-----------------------------------------------------------------------------

    // Get/Set the debug draw mode of the graph.
    FORCEINLINE EMerGridDebugDrawMode GetDebugDrawMode() const { return DebugDrawMode; }
    FORCEINLINE FMercunaAgentType GetDebugDrawAgentType() const { return DebugDrawAgentType; }
    void SetDebugDrawMode(EMerGridDebugDrawMode Mode);
    void SetDebugDrawAgentType(const FMercunaAgentType& AgentType);
    void SetDebugDrawMode(EMerGridDebugDrawMode Mode, EMerGridDebugDrawSubmode Submode, const FMercunaAgentType& AgentType);

    void DebugDraw();

    //-----------------------------------------------------------------------------

    virtual uint32 GetOrAddMaterialId(const UPhysicalMaterial* Material) override;

    //-----------------------------------------------------------------------------

    virtual bool IsNavigationRelevant(const AActor* Actor) const override;
    virtual bool IsNavigationRelevant(const UActorComponent* Component) const override;

    //-----------------------------------------------------------------------------

    virtual void PostLoad() override;
    virtual void PostRegisterAllComponents() override;
    virtual void BeginDestroy() override;
    virtual void Serialize(FArchive& Ar) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

    void UpdateNavLink(class AMercunaNavLink* NavLink);

protected:
    friend mercuna::MerPtr<mercuna::IMerNavGraph2D> mercuna::GetMerNavGraph2D(AMercunaNavGraph2D* NavGraph2D);
    friend mercuna::MercunaNavGraph2DImpl;
    friend AMerNavDataChunk;
    friend AMercunaNavLink;

    /** Which type of agents does this grid support. For each agent type a new subgrid 
    * will be generated.
    */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, meta = (ExposeOnSpawn = true))
    FMercunaSupportedAgentTypes SupportedAgentTypes;

    /** Whether to use nav seeds.  If false then nav seeds are not required but enclosed
     * spaces aren't removed from the nav graph, increasing memory usage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, meta = (ExposeOnSpawn = true))
    bool bUseNavSeeds;

    // Current debug draw mode
    UPROPERTY(Transient)
    EMerGridDebugDrawMode DebugDrawMode;

    UPROPERTY(Transient)
    EMerGridDebugDrawSubmode DebugDrawSubmode;

    UPROPERTY(Transient)
    FMercunaAgentType DebugDrawAgentType;

    UPROPERTY()
    TArray<TObjectPtr<AMercunaNavLink>> NavLinks;

    UPROPERTY()
    uint32 NextMaterialId;

    UPROPERTY()
    TMap<TObjectPtr<const UPhysicalMaterial>, uint32> SurfaceMaterials;

    UPROPERTY()
    FVector NavLinkBorder = FVector(0.0);

    // Returns the internal impl that contains Mercuna types
    mercuna::MercunaNavGraph2DImpl* GetImpl() const;

    // What type of 2D nav graph is this
    virtual EMercunaGridType GetGridType() const { return EMercunaGridType::None; }

    // Set up an empty graph
    virtual void SetupEmpty() override;

    // Calculate the aligned (with global generation grid) position
    virtual FVector GetAlignedOrigin(const FVector& Pos, const FQuat& Orientation) const;

    // Check whether the properties of the underlying grid match those in the project settings
    virtual void ValidateConfiguration() {}

    virtual void SavePartTemp(AMerNavDataChunk* DataChunk) override;

    void RelinkNavLinksInBounds(const FBox& NavLinkCollectionBounds);

    // Merge the data from a chunk into this graph
    virtual bool AddNavDataChunk(AMerNavDataChunk* DataChunk) override;

    // Calculate how far extra need to search for navlinks beyond a volume to be
    // sure that all the ones that can join to the grid are found
    virtual void CalculateNavlinkBorder() {}

    // Attempt to reassociate a navlink with internal link
    void RelinkNavLink(AMercunaNavLink* NavLink);

    // Remove nav link from this nav grid, used when nav link is being unloaded
    void RemoveNavLink(AMercunaNavLink* NavLink);

    // Find all the navlinks related to this grid and reassociate them with internal links
    void RelinkNavLinks(const TArray<AMercunaNavLink*>& NewNavLinks);

    // Remove nav links from this nav grid, used when purging nav links when a level is being unloaded
    void RemoveNavLinks();

    // Load materials properties into internal grid
    void LoadMaterials();

    // Populate the surface material map with the default and priority material
    void PopulateMaterials();

    // Add a material to the internal nav graph
    void AddMaterialToNavGraph(uint8_t MaterialId, const UPhysicalMaterial* Material);

    // Trigger rebuild changes
    void TriggerRebuildChanges() override;

    // Modifier volume rotation is allowed
    virtual bool AllowModifierVolumeRotation() const override { return true; }
};
