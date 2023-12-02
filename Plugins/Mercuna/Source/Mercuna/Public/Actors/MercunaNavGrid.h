// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Actors/MercunaNavGraph2D.h"

#include "MercunaTypes.h"

#include "MercunaNavGrid.generated.h"

// Forward declarations
class AMercunaNavGrid;
class AMercunaNavLink;
class UMercunaPath;
class UMercunaSmoothPath;
class AMercunaNavInvokerVolume;
class AMerNavDataChunk;

namespace mercuna
{
    struct MercunaNavGridImpl;
    struct MerUsageSpec;
}

//-----------------------------------------------------------------------------

/** Line segment representing an edge of navigable space.  Orientation is such that the clockwise end around navigable space is at the start and the anti-clockwise end at the end **/
USTRUCT(BlueprintType)
struct FMercunaNavMeshEdge
{
    GENERATED_USTRUCT_BODY()

    /** Position of the start, or clockwise side, of the edge around navigable space */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mercuna")
    FVector EdgeStart = FVector::ZeroVector;

    /** Position of the end, or anti-clockwise side, of the edge around navigable space */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mercuna")
    FVector EdgeEnd = FVector::ZeroVector;
};

USTRUCT()
struct FMercunaNavInvoker
{
    GENERATED_BODY()

    // The actor that acts as the nav invoker
    TWeakObjectPtr<AActor> Actor;

    // The radius within which nav grid will be generated
    float GenerationRadius;

    // The radius at which nav grid will be removed
    float RemovalRadius;

    // Which agent type does this invoker apply to
    int AgentTypeIdx;
};

//-----------------------------------------------------------------------------

/** Ground based 2.5D navigation graph on which pawns with a MercunaGroundNavigationComponent can move */
UCLASS(hidecategories = (Input, Physics, Rendering, Tags, Actor, Layers, LOD, Cooking, Collision))
class MERCUNA_API AMercunaNavGrid : public AMercunaNavGraph2D
{
    GENERATED_UCLASS_BODY()

public:
    virtual ~AMercunaNavGrid() override;

    /** If set, then stationary obstacles will be cut out of the nav graph instead of being treated as
     * dynamic obstacles. */
    UPROPERTY(EditAnywhere, Category = Runtime, config)
    bool bCutOutStationaryObstacles;

    //-----------------------------------------------------------------------------

    /** Whether jump links should be automatically generated for this nav graph. Jump links are only generated
     * during builds. They are not generated during rebuilds or when using nav invokers. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, config, meta = (ExposeOnSpawn = true, DisplayAfter="bUseNavSeeds"))
    bool bGenerateJumpLinks;

    /** This fraction of the agent radius controls perching over non-vertical steps, allowing navigation 
    * over rough surfaces. Navigation can extend across a steep slope for up to this distance. Increasing 
    * the value causes the nav graph to cover a larger area, but some areas may incorrectly be considered
    * navigable.
    */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generation, AdvancedDisplay, meta = (ExposeOnSpawn = true, ClampMin = "0.0", ClampMax = "0.5", DisplayAfter = "GeometryCollisionChannel"))
    float RoughSurfacePerchFraction = 0.3f;

    //-----------------------------------------------------------------------------

    /** Checks whether a position is in navigable space
    * @param AgentType          Which agent sub grid to query
    * @param Position           The position to clamp
    * @param Result             True if the position is in navigable space
    * @param UsageSpec          Specifies the valid usage flags for the query
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void IsNavigable(const FMercunaGroundAgentType& AgentType, FVector Position, bool& Result, FMercunaPawnUsageFlags UsageSpec = FMercunaPawnUsageFlags());

    /** Checks whether multiple positions are in navigable space
    * @param Positions          The positions to clamp
    * @param Results            True if a position is in navigable space
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void IsNavigableMulti(const FMercunaGroundAgentType& AgentType, const TArray<FVector>& Positions, TArray<bool>& Results);

    /** Clamp a position to navigable space.
    * @param Position           The position to clamp
    * @param SearchRadius       The maximum horizontal distance to search for a valid position
    * @param HeightRange        The maximum vertical distance to search for a valid position
    * @param ClampedPosition    The position after it has been clamped
    * @param Result             True if the position has been successfully clamped to navigable space, false if no valid space within SearchRadius
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void ClampToNavigable(const FMercunaGroundAgentType& AgentType, FVector Position, float SearchRadius, float HeightRange, FVector& ClampedPosition, bool& Result);

    /** Project a position vertically on to navigable space.
    * @param Position           The position to project
    * @param Direction          The direction to project in.
    * @param ClampedPosition    The position after it has been clamped
    * @param Result             True if the position has been successfully clamped to navigable space, false if no valid space within SearchRadius
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void ProjectToNavigable(const FMercunaGroundAgentType& AgentType, FVector Position, EMercunaProjectionDirection Direction, FVector& ClampedPosition, bool& Result);

    /** Perform a raycast through the navigable grid. Result is true if the raycast
    *   succeeds, otherwise HitPosition is set to the position of the first hit
    * @param Start              The position to raycast from
    * @param End                The position to raycast to
    * @param HitPosition        The position that the ray hits
    * @param RayHit             True if the ray hits something, false if the ray is clear
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void Raycast(const FMercunaGroundAgentType& AgentType, FVector Start, FVector End, FVector& HitPosition, bool& RayHit);

    /** Check whether there is a path from Start to End. Returns true if path can be found
    * @param Start              The position to path from
    * @param End                The position to path to
    * @param MaxPathLength      The maximum length the path is allowed to be
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    bool IsReachable(const FMercunaGroundAgentType& AgentType, FVector Start, FVector End, float MaxPathLength = 1000.f);

    /** Check whether there is a path from Start to each of the EndPos positions using a Latent action.
     * Calls back with the results, this call will be made on a worker thread.
     * @param Start             The position to path from
     * @param EndPos            The positions to path to
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
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (Latent, LatentInfo = "LatentInfo", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    void IsReachableLatent(const FMercunaGroundAgentType& AgentType, FVector Start, const TArray<FVector>& EndPos,
                           TArray<bool>& Result, TArray<FVector>& ClampedPos, UObject* WorldContextObject, struct FLatentActionInfo LatentInfo,
                           float MaxPathLength = 1000.f, bool AccuratePathLength = true, float StartSearchRadius = -1.f, float EndSearchRadius = 0.f);

    /** Asynchronously check whether there is a path from Start to each the the EndPos positions.
     * Calls back with the results, this call will be made on a worker thread.
     * @param Start             The position to path from
     * @param EndPos            The positions to path to
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
     * @param UsageSpec          Specifies the valid usage flags for the query
     */
    void IsReachableAsync(const FMercunaGroundAgentType& AgentType, FVector Start, const TArray<FVector>& EndPos, float MaxPathLength, bool AccuratePathLength,
                          TMercunaReachableCallback OnCompleteCallback, float StartSearchRadius = -1.f, float EndSearchRadius = 0.f, FMercunaPawnUsageFlags UsageSpec = FMercunaPawnUsageFlags());

    /** Generate a set of random points that are within specified path length of the Start position.
     * If the Start position cannot be clamped to navigable space then no points will be generated.
    * @param AgentType          Which agent sub grid to query
    * @param Start              The position to generate points from
    * @param MaxPathLength      The maximum path distance points can be from the Start position
    * @param NumPoints          The number of points to generate
    * @param Results            The generated random points
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void RandomInRange(const FMercunaGroundAgentType& AgentType, FVector Start, float MaxPathLength, int NumPoints, TArray<FVector>& Results);

    /** Gets the polymesh edges within a given range of the provided location
    * @param  AgentType      Which agent sub grid to query
    * @param  Start          The position to find edges near
    * @param  SearchRadius   The maximum distance to the closest point on a returned edge
    * @param  NavMeshEdges   The edges of the navmesh, oriented such that the clockwise end around navigable space is at the start and the anti-clockwise end at the end
    * @param  UsageSpec      Specifies the valid usage flags for the query
    * @param  QueryingActor  Optional actor performing the query, relevant if stationary object status might affect edges
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void NavMeshEdgesInRange(const FMercunaGroundAgentType& AgentType, const FVector& Start, const float SearchRadius,
                             TArray<FMercunaNavMeshEdge>& NavMeshEdges,
                             FMercunaPawnUsageFlags UsageSpec = FMercunaPawnUsageFlags(), AActor* QueryingActor = nullptr);

    /** Gets the closest polymesh edge to the provided location and the closest point along it
    * @param  AgentType          Which agent sub grid to query
    * @param  Position           The position to find edges near
    * @param  Result             The closest edge to the specified point, oriented such that the clockwise end around navigable space is at the start and the anti-clockwise end at the end
    * @param  ClosestPoint       The closest point along the edge
    * @param  ClosestPointFound  @p true if a point was found within range, @p false otherwise
    * @param  UsageSpec          Specifies the valid usage flags for the query
    * @param  QueryingActor      Actor performing the query, relevant if stationary object status might affect edges
    * @return                    @p true if at least one edge within range was found, @p false otherwise
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void GetClosestNavMeshEdge(const FMercunaGroundAgentType& AgentType, const FVector& Position,
                               FMercunaNavMeshEdge& Result, FVector& ClosestPoint, bool& ClosestPointFound,
                               FMercunaPawnUsageFlags UsageSpec = FMercunaPawnUsageFlags(), AActor* QueryingActor = nullptr);

    //-----------------------------------------------------------------------------

    /** Check whether the nav grid is built for a given agent type at a given position */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    bool IsBuilt(const FMercunaGroundAgentType& AgentType, const FVector& Pos) const;

    /** Check whether the nav grid is built for a given agent type within a given box */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    bool IsBuiltWithinBox(const FMercunaGroundAgentType& AgentType, const FBox& Box) const;

    //-----------------------------------------------------------------------------

    /** Save the nav grid to the specified file in obj model format. Overwrites the file if it already exists.
     * Should only be used for debugging */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (DevelopmentOnly))
    void SaveToObjFile(FMercunaGroundAgentType AgentType, bool bDrawPolyMesh = false) const;

    //-----------------------------------------------------------------------------

    /** Create the grid. Used at runtime as an alternative to building the grid.
     * The grid is initialized but not populated, RebuildVolume(s) can then be used to build sections of the grid. */
    UE_DEPRECATED(2.3, "No longer required")
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (DeprecatedFunction, DeprecationMessage = "No longer required"))
    void Create() {}

    //-----------------------------------------------------------------------------

    // Register an actor to act as a nav invoker. Navigation grid will be generated around
    // this actor as it moves within the specified radius
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void RegisterNavInvoker(AActor* Actor, FName AgentType, float GenerationRadius = 3000.0f, float RemovalRadius = 5000.0f);
    void RegisterNavInvoker(AActor* Actor, FMercunaGroundAgentType AgentType, float GenerationRadius = 3000.0f, float RemovalRadius = 5000.0f);

    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void UnregisterNavInvoker(AActor* Actor);

    // If using nav invokers how often should the grid to updated (in seconds).
    UPROPERTY(EditAnywhere, Category = Generation, AdvancedDisplay)
    float NavInvokerUpdateFrequency = 1.0f;

    // Will be used internally by the invoker volume actors, and shouldn't be called directly!
    void AddNavInvokerVolume(AMercunaNavInvokerVolume* NavInvokerVolume);
    void RemoveNavInvokerVolume(AMercunaNavInvokerVolume* NavInvokerVolume);

    //-----------------------------------------------------------------------------

    // Get the most suitable grid
    static AMercunaNavGrid* GetBestInstance(UWorld* World, const FVector& Position, const FMercunaGroundAgentType& AgentType);

    // Create a new NavGrid in the specified world and level
    static AMercunaNavGrid* CreateInstance(UWorld* World, ULevel* Level);

    //-----------------------------------------------------------------------------

    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaPath* FindPathToLocation(const FMercunaGroundAgentType& AgentType, const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, FVector StartDir, FVector End,  float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);
    UMercunaPath* FindPathToLocation(const FMercunaGroundAgentType& AgentType, FVector Start, FVector StartDir, FVector End, float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaPath* FindPathToLocations(const FMercunaGroundAgentType& AgentType, const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, FVector StartDir, const TArray<FVector>& Destinations, float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);
    UMercunaPath* FindPathToLocations(const FMercunaGroundAgentType& AgentType, FVector Start, FVector StartDir, const TArray<FVector>& Destinations, float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaSmoothPath* FindSmoothPathToLocation(const FMercunaGroundAgentType& AgentType, const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, FVector StartDir, FVector End, float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, float MaxSpeedReverse = 250.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);
    UMercunaSmoothPath* FindSmoothPathToLocation(const FMercunaGroundAgentType& AgentType, FVector Start, FVector StartDir, FVector End, float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, float MaxSpeedReverse = 250.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AutoCreateRefTerm = "OnPathUpdated"))
    UMercunaSmoothPath* FindSmoothPathToLocations(const FMercunaGroundAgentType& AgentType, const FOnMercunaPathUpdated& OnPathUpdated, FVector Start, FVector StartDir, const TArray<FVector>& Destinations, float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, float MaxSpeedReverse = 250.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);
    UMercunaSmoothPath* FindSmoothPathToLocations(const FMercunaGroundAgentType& AgentType, FVector Start, FVector StartDir, const TArray<FVector>& Destinations, float MinTurnRadius = 100.0f, float IdealTurnRadius = 200.0f, float MaxSpeed = 1000.0f, float MaxSpeedReverse = 250.0f, bool AllowPartial = false, float MaxPathLength = 10000.0f, float SlopePenalty = 1.0f);

    // Internal versions of path find to explicitly request debug draw
    UMercunaPath* FindPathToLocationWithDebug(const FMercunaGroundAgentType& AgentType, FVector Start, FVector StartDir, FVector End, FVector EndDir, const TArray<FVector>& Waypoints, float MinTurnRadius, float IdealTurnRadius, float MaxSpeed, float InitialSpeed, bool AllowPartial, float MaxPathLength, float SlopePenalty, bool SetPathfindDebugOnNavGrid, const mercuna::MerUsageSpec& UsageSpec, const FOnMercunaPathUpdated& OnPathUpdatedDelegate = FOnMercunaPathUpdated());
    UMercunaSmoothPath* FindSmoothPathToLocationWithDebug(const FMercunaGroundAgentType& AgentType, FVector Start, FVector StartDir, FVector End, FVector EndDir, const TArray<FVector>& Waypoints, float MinTurnRadius, float IdealTurnRadius, float MaxSpeed, float InitialSpeed, float MaxSpeedReverse, bool AllowPartial, float MaxPathLength, float SlopePenalty, bool SetPathfindDebugOnNavGrid, const mercuna::MerUsageSpec& UsageSpec, const FOnMercunaPathUpdated& OnPathUpdatedDelegate = FOnMercunaPathUpdated());

    //-----------------------------------------------------------------------------

    /** Set a callback that is called every time a nav link is searched in a nav grid query. The callback allows game code
     * to decide whether the link can be used. Queries which use the callback include path finds (standard and kinematic)
     * and reachability tests.
     * 
     * The callback will be called by queries that are run on task threads. Therefore the callback MUST be thread-safe.
     * It will be called once for each nav link encountered in a query so must be very high performance.
     * The returned weak pointers cannot be dereferenced but only used for comparison (using HasSameIndexAndSerialNumber).
     * If any nav link queries are running, the Set function will block until they are finished.
     *
     * The arguments of the callback are:
     * @param Traverser             A weak pointer to the query actor. This will be null if the query is from a location
     * @pamam AgentType             The query agent type
     * @param NavLink               A weak pointer to the nav link actor being tested
     * @param NavLinkStart          The position of the start of the nav link, relative to the nav grid transform
     * @param NavLinkEnd            The position of the end of the nav link, relative to the nav grid transform
     * @param Direction             Whether the nav link is being traversed left-to-right or right-to-left
     */
    typedef TFunction<bool(const TWeakObjectPtr<AActor>&, const FMercunaAgentType&, const TWeakObjectPtr<AActor>&, const FVector&, const FVector&, EMercunaNavigationLinkDirection)> TCanTraverseNavLinkCallback;
    void SetNavLinkTraversalCallback(const TCanTraverseNavLinkCallback& Callback);

    /** Clear nav link traversal callback. This MUST be done before the game code actor
     * hosting the call back is destroyed (unless the nav grid has been fully destroyed first).
     * If any nav link queries are running, the function will block until they are finished. */
    void ClearNavLinkTraversalCallback();

    //-----------------------------------------------------------------------------

#if WITH_EDITOR
    /** Load the nav grid from a file. Should only be used for debugging. Can't be used in PiE/SiE modes. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (DevelopmentOnly))
    bool LoadFromFile(FString Filename);
#endif

    //-----------------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void PostActorCreated() override;
    virtual void Tick(float DeltaSeconds) override;

    virtual void SetNavigationOrientation(const FQuat& Orientation) override;

    virtual FBox GetNavVolumeBounds() const override;

    virtual bool CutOutStationaryObstacles() const override;

    void GetMeshData(FArchive& Ar, const FVector& ViewPos);
    void SetMeshData(FArchive& Ar);
    bool IsDebugDrawDirty() const;
    
private:
    friend mercuna::MercunaNavGridImpl;

    // Set up an empty graph
    virtual void SetupEmpty() override;

    // What type of 2D nav graph is this
    virtual EMercunaGridType GetGridType() const override { return EMercunaGridType::Ground; }

    virtual FVector GetAlignedOrigin(const FVector& pos, const FQuat& orientation) const override;

    // Get a nav grid with matching settings to this nav grid.
    AMercunaNavGraph* GetMatchingNavGraph() const override;

    // Trigger a merge of this nav grid into another nav grid
    void DoMerge(AMercunaNavGraph* pOtherNavGraph) override;

    // Transfer master from another nav grid on unload of old master.
    void TransferMaster(AMercunaNavGraph* pFromNavGraph) override;

#if WITH_EDITOR
    // Calculate how far extra geometry needs to be collected so that the edges
    // of the graph are correct when only building part of the graph
    virtual FVector CalculateRequiredBuildPadding() const override;
#endif

    // Check whether the properties of the underlying grid match those in the project settings
    virtual void ValidateConfiguration() override;

    // Find the current position of all nav invokers and queue updates to the nav grid
    void UpdateNavInvokers();

    // Determines the cell size and height to use based on agent configuration
    // Returns true if at least one agent type is configured, false otherwise.
    bool GetCellSizeAndHeight(float& CalculatedCellSize, float& CalculatedCellHeight) const;

    virtual void CalculateNavlinkBorder() override;

    // Whether nav volumes should always be added/removed from the underlying internal nav graph immediately,
    // instead of waiting for the next build.
    virtual bool AlwaysModifyNavVolumesImmediately() const override;

    virtual void SavePart(FArchive& Ar, const FBox& Bounds, TMap<TObjectPtr<const UPhysicalMaterial>, uint32>& PartSurfaceMaterials) const override;

    UPROPERTY(Transient)
    TArray<FMercunaNavInvoker> NavInvokers;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AMercunaNavInvokerVolume>> NavInvokerVolumes;

    // If nav invokers are enabled when they were last updated
    double LastNavInvokerUpdateTime = -100.0;
};
