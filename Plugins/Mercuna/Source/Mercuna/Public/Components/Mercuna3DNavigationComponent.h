// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaNavigationComponent.h"

#include "Mercuna3DNavigationComponent.generated.h"

// Forward declarations
namespace mercuna
{
    struct MerSteeringParams3D;
    struct Mercuna3DNavigationComponentImpl;
    class IMerPilot;
}
class UMercunaObstacleComponent;
class AMercunaNavOctree;

//-----------------------------------------------------------------------------

USTRUCT()
struct MERCUNA_API FMercunaMoveRequest3D : public FMercunaMoveRequest
{
    GENERATED_USTRUCT_BODY()

        FMercunaMoveRequest3D()
        : FMercunaMoveRequest()
    {}

    FMercunaMoveRequest3D(const AActor* InGoalActor, float InEndDistance, float InSpeed, bool bInUsePartialPath, bool bAvoidGoalActor)
        : FMercunaMoveRequest(InGoalActor, InEndDistance, InSpeed, bInUsePartialPath, bAvoidGoalActor, FVector::ZeroVector)
    {}

    FMercunaMoveRequest3D(const FVector& InGoalLocation, float InEndDistance, float InSpeed, bool bInUsePartialPath)
        : FMercunaMoveRequest(InGoalLocation, InEndDistance, InSpeed, bInUsePartialPath, FVector::ZeroVector)
    {}
};

//-----------------------------------------------------------------------------

UENUM()
enum class EMercunaMoveInForwardDirection : uint8
{
    Always          UMETA(DisplayName = "Always"),
    Prefer          UMETA(DisplayName = "Prefer"),
    Independent     UMETA(DisplayName = "Independent")
};

/** Configuration parameters used by the 3D navigation component that controls movement style */
USTRUCT(BlueprintType)
struct FMercuna3DNavigationConfiguration
{
    GENERATED_USTRUCT_BODY()

    /** The default maximum pitch angle for orientation, in degrees.
    * Note: This is ignored if Move In Forward Direction is set to Always. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style", meta = (ClampMin = "0.0", ClampMax = "90.0"))
    float MaxPitch;

    /** The maximum roll when turning, in degrees. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style", meta = (ClampMin = "0.0", ClampMax = "90.0"))
    float MaxRoll;

    UPROPERTY(meta = (DeprecatedProperty, DeprecationMessage="From v2.0 use MoveInForwardDirection instead"))
    bool MoveOnlyInLookDirection = false;

    /** Whether to restrict movement to forward direction */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style")
    EMercunaMoveInForwardDirection MoveInForwardDirection;

    /** Whether should slow down in order to stop at destination */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style")
    bool StopAtDestination;

    // How should this pawn react to an approaching obstacle when it is stopped
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style")
    EMercunaAvoidanceWhenStationary AvoidanceWhenStationary;

    /** Whether to prefer staying at the same height. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float HeightChangePenalty;

    /** Whether to use smooth spline based paths, rather than simple polyline paths */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style")
    bool SmoothPaths;

    /** When following a smooth path the look ahead point is set to be this far down the spline (in s). */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "Movement Style", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float LookAheadTime;

    /** Time before a turn to start rolling (only applied with SmoothPaths enabled) */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "Movement Style", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float RollAnticipationTime;

    /** Defaults */
    FMercuna3DNavigationConfiguration()
    {
        MaxPitch = 10.0f;
        MaxRoll = 0.0f;
        MoveInForwardDirection = EMercunaMoveInForwardDirection::Prefer;
        StopAtDestination = true;
        AvoidanceWhenStationary = EMercunaAvoidanceWhenStationary::None;
        HeightChangePenalty = 0.0f;
        SmoothPaths = true;
        LookAheadTime = 1.0f;
        RollAnticipationTime = 0.1f;
    }
};

//-----------------------------------------------------------------------------

/** A component for moving a pawn using the Mercuna 3D navigation system */
UCLASS(ClassGroup = Mercuna, meta = (DisplayName = "Mercuna 3D Navigation Component", BlueprintSpawnableComponent, ShortTooltip = "3D Navigation component"))
class MERCUNA_API UMercuna3DNavigationComponent : public UMercunaNavigationComponent
{
    GENERATED_UCLASS_BODY()

public:

    /** Move toward Destination location
    * @param Destination    The position to move to
    * @param EndDistance    The distance from the destination that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath  Whether to move as close as possible to the destination if no path to the destination can be found
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void MoveToLocation(const FVector& Destination, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = true);

    /** Move to multiple destination locations
    * @param Destinations   Array of position to move through in turn
    * @param EndDistance    The distance from the final destination that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath  Whether to move as close as possible to the destinations if no path to them can be found
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void MoveToLocations(const TArray<FVector>& Destinations, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = true);

    /** Add an additional destination to the end of the current path.  The agent must be moving to a location. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void AddDestinationLocation(FVector NextDestination);
    
    /** Move toward Actor. While the path will update as the target Actor moves, once Pawn is within range the 
    *   action will finish.
    * @param Actor          The target actor to move to
    * @param EndDistance    The distance from the target actor that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath  Whether to move as close as possible to the destination if no path to the destination can be found
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void MoveToActor(AActor* Actor, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = true);

    /** Follow specified Actor trying to stay within a specific distance of the target. This action never completes.
    * @param Actor          The target actor to follow
    * @param Distance       The distance from the target actor that the Pawn tries to stay within
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param Offset         The offset from the tracked actor in the tracked actor's local frame.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (AdvancedDisplay = "Offset"))
    void TrackActor(AActor* Actor, float Distance = 0.0f, float Speed = 1.0f, FVector Offset = FVector::ZeroVector);

    /** Request that the pawn follows another actor */
    virtual FAIRequestID RequestTrackActor(AActor* Actor, float Distance = 0.0f, float Speed = 1.0f, FVector Offset = FVector::ZeroVector, bool bOffsetInWorldSpace = false, bool bAvoidTrackActor = false);

    /** Bring the Pawn to a complete stop as rapidly as possible. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void Stop();
    
    /** Request that the Pawn moves to specified goal or actor */
    FAIRequestID RequestMoveTo(const FMercunaMoveRequest3D& MoveRequest);
    
    /** Request that the Pawn comes to a complete stop as soon as possible */
    FAIRequestID RequestStop() override;

    /** If already doing a MoveTo action update the destination. Returns false if the MoveTo action can't be updated. */
    bool UpdateMoveToLocation(FAIRequestID RequestID, FVector NewDestination) override;

    /** Add an extra destination when running a MoveTo action. Returns false if the MoveTo action can't be updated. */
    bool AddMoveToLocation(FAIRequestID RequestID, FVector NextDestination) override;

    /** Terminate any active movement actions */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void CancelMovement();
    void CancelMovement(FAIRequestID RequestId) override;

    /** Blueprint notification that the pawn has completed the current movement request */
    UPROPERTY(BlueprintAssignable)
    FMercunaMoveComplete OnMoveCompleted;

    /** If the pawn is following a path then get the approximate distance to the end of the path
    *   This may return that the path is not valid if it is still being computed.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void GetPathInfo(bool& Valid, float& DistanceToEnd);
    
    /** Get remaining path length.
     * Returns straight line distance if the path is not yet available.
     * Returns zero if there is no active move request. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    float GetRemainingPathLength();

    /** Look at a target Actor
     * @param Actor        The target actor to look at
     * @param MaxPitch     Maximum pitch angle override, in degrees.  A negative value means don't override. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void LookAt(AActor* Actor, float MaxPitch = -1.f);

    /** Look along a particular direction vector
     * @param Direction    The direction to look in 
     * @param MaxPitch     Maximum pitch angle override, in degrees.  A negative value means don't override. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void LookInDirection(FVector Direction, float MaxPitch = -1.f);

    /** Look along acceleration vector
     * @param MaxPitch     Maximum pitch angle override, in degrees.  A negative value means don't override. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void LookAlongAcceleration(float MaxPitch = -1.f);

    /** Cancel look at target/direction. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void CancelLookAt();

    /** Check to see if a nearby point is reachable via a path through the nav octree.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta=(Latent, LatentInfo = "LatentInfo", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    void IsReachable(FVector Point, bool& Success, UObject* WorldContextObject, struct FLatentActionInfo LatentInfo);

    /** Pauses control of this pawn, this should be used when Game logic is taking control of this pawn's movement.
    * This does not clear the navigation command.
    * Navigation is automatically resumed when any move request is made.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void PauseNavigation();

    /** Resume navigation. Used to continue navigating after a call to PauseNavigation. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void ResumeNavigation();

    /** Disable and re-enable avoidance against a particular actor. The actor needs to have a 
    *   MercunaObstacleComponent to be considered as an obstacle in the first place. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    virtual void SetAvoidanceAgainst(AActor* Actor, bool Enable = false) override;

    /** Reset avoidance excluded actors */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    virtual void ClearAvoidanceExclusions() override;

    /** Reconfigure the navigation component to switch pathfinding and avoidance on or off. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void ConfigureMovement(bool bUsePathfinding = true, EMercunaAvoidanceMode NewAvoidanceMode = EMercunaAvoidanceMode::ORCA);

    /** Reconfigure the navigation component to switch parameters controlling flight style. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void Configure(const FMercuna3DNavigationConfiguration& NewConfiguration);

    /** Set the movement component to be used */
    void SetMovementComponent(class IMercuna3DMovementInterface* NewMovementComponent);

    /** Set the octree that pawn should use */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void SetNavOctree(AMercunaNavOctree* NavOctree);

    /** Choose the best available nav octree for this pawn at its current location */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    void SetNavOctreeToBest();

    /** Get the octree this pawn is using */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    AMercunaNavOctree* GetNavOctree() const { return NavOctree; }
    AMercunaNavGraph* GetNavGraph() const override;

    /** Get the size of this agent for navigation */
    float GetNavigationRadius() const { return NavigationRadius; }

    /** Set the usage flags for the pawn */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation")
    virtual void SetUsageFlags(FMercunaPawnUsageFlags NewUsageFlags) override;

    FORCEINLINE const FMercuna3DNavigationConfiguration& GetConfig() const { return Configuration; }

    // Is this actor currently running dynamic avoidance
    // If an actor (that has an obstacle component) is specified then checks whether that particular 
    // actor is avoided
    bool DoesAvoidance(AActor* Actor = nullptr) const override;

    /** Get the spline that the pawn is currently following, if any.
      If the pawn is not following a path, or the Smooth Paths configuration option is turned off, 
      then an empty spline is returned. */
    virtual FSplineCurves GetCurrentSpline() const override;

    //-------------------------------------------------------------------------

    virtual ENavType GetType() const override { return ENavType::ThreeDimensional; }

    void SetNavGraph(AMercunaNavGraph* pNavGraph) override;
    void SetNavGraphToBest() override { SetNavOctreeToBest(); }

    virtual void Activate(bool bReset = false) override;
    virtual void Deactivate() override;

    virtual void PostLoad() override;
    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
    virtual bool HasSuccessfullyInitialized() const override;

    virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
    friend class FMercuna3DNavigationComponentVisualizer;

    /** Configuration properties defining the movement style of the pawn */
    UPROPERTY(Category = "Configuration", EditAnywhere, BlueprintReadOnly, meta = (ShowOnlyInnerProperties))
    FMercuna3DNavigationConfiguration Configuration;

    /** The octree for this pawn to use. If not set then the most suitable octree will be automatically used */
    UPROPERTY(EditAnywhere, Category = Navigation)
    TObjectPtr<AMercunaNavOctree> NavOctree;

    /** Whether the navigation radius should be automatically calculated from the pawn collision */
    UPROPERTY(EditAnywhere, Category = Navigation)
    bool bAutomaticRadius;

    /** The size to consider this pawn as for the purposes of navigation */
    UPROPERTY(EditAnywhere, Category = Navigation, meta=(ClampMin = "0.1", EditCondition = "!bAutomaticRadius"))
    float NavigationRadius;

    /** Whether to do proper pathfinding to the destination */
    UPROPERTY(EditAnywhere, Category = Navigation)
    bool Pathfinding;

    // Current look target and max allowed pitch (which may have been overridden from the default)
    TWeakObjectPtr<AActor> LookTarget;
    FVector LookDir{ ForceInitToZero };
    bool LookAtAccel{ false };
    float MaxPitch = 0.0f;

    /** Record debug info (logs and debug draw) about the pawn */
    virtual void UpdateDebugInfo(float DeltaTime);

    /** Called every frame to query the pilot for desired movement and pass output to movement component */
    virtual void ProcessMovement(float DeltaTime);

    /** Check that the pawn is in navigable space. If no octree is set then attempt to 
    find the best ones to use */
    virtual bool CheckNavigable();

    virtual void OnMoveCompleteDelegate(const FAIRequestID& RequestId, EMercunaMoveResult::Type result) override;

    /** Construct a MerEntity and return a pointer to it. */
    virtual mercuna::MerNavigatingEntityPtr GetMerEntity() const override;

    /** Get the internal navigator */
    virtual mercuna::MerPtr<mercuna::IMerNavigator> GetNavigator() const override;

    /** Get the internal pilot */
    mercuna::MerPtr<mercuna::IMerPilot> GetPilot() const;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;

private:
    /** The desired roll of the pawn last frame. */
    float m_lastRoll;

    /** The movement component that is driven by this navigation component. */
    class IMercuna3DMovementInterface* MovementComponent;

    /** Hides the private Mercuna internal types. Is only created when component is initialized */
    mercuna::Mercuna3DNavigationComponentImpl* m_pImpl;

    /** Update the movement properties of the navigation component. This is automatically called (via a delegate)
    *   by the movement component when a property changes. */
    void UpdateMovementProperties();

    /** Set an orientation change and desired angular velocity down to the movement component, respecting
     * configured max rotation rates. */
    void SetDesiredOrientationChange(const FQuat& orientChange, const FVector& desiredAngVel, const float DeltaTime, const mercuna::MerSteeringParams3D& steeringParams);
};
