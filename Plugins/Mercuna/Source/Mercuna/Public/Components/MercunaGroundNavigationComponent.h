// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaNavigationComponent.h"
#include "Animation/MotionTrajectoryTypes.h"

#include "MercunaGroundNavigationComponent.generated.h"

// Forward declarations
namespace mercuna
{
    struct MercunaGroundNavigationComponentImpl;
    struct MerSteeringParams2D;
    class IMerDriver;
}
class AMercunaNavGraph2D;
class AMercunaNavLink;
class UPawnMovementComponent;

//-----------------------------------------------------------------------------

USTRUCT()
struct MERCUNA_API FMercunaMoveRequestGround : public FMercunaMoveRequest
{
    GENERATED_USTRUCT_BODY()

        FMercunaMoveRequestGround()
        : FMercunaMoveRequest()
        , EndDirection(FVector::ZeroVector)
    {}

    FMercunaMoveRequestGround(const AActor* InGoalActor, float InEndDistance, float InSpeed, bool bInUsePartialPath, bool bAvoidGoalActor)
        : FMercunaMoveRequest(InGoalActor, InEndDistance, InSpeed, bInUsePartialPath, bAvoidGoalActor, FVector::ZeroVector)
        , EndDirection(FVector::ZeroVector)
    {}

    FMercunaMoveRequestGround(const FVector& InGoalLocation, const FVector& InGoalDirection, float InEndDistance, float InSpeed, bool bInUsePartialPath)
        : FMercunaMoveRequest(InGoalLocation, InEndDistance, InSpeed, bInUsePartialPath, FVector::ZeroVector)
        , EndDirection(InGoalDirection)
    {}

    // Direction of the agent at the destination (zero indicates any)
    FVector EndDirection;
};

//-----------------------------------------------------------------------------

/** Parameters that control how Mercuna steers ground pawns */
USTRUCT(BlueprintType)
struct FMercunaGroundNavigationSteeringParameters
{
    GENERATED_USTRUCT_BODY()

    // Whether the pawn should use kinematic pathfinding that takes turn rates
    // into account  or use standard A* pathfinding
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
    bool bKinematicPathfinding;

    // The maximum speed that the pawn can travel at
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering", meta = (ClampMin = "0.0"))
    float MaxSpeed;

    // The maximum acceleration in all directions for characters and animals
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering", meta = (ClampMin = "0.0"))
    float MaxAcceleration;

    // Radius of the turns a pawn should try to make when moving
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering", meta = (ClampMin = "1.0"))
    float IdealTurningRadius;

    // Radius of the tightest turn an animal can make when moving
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering", meta = (ClampMin = "1.0"))
    float MinTurningRadius;

    // The maximum forward acceleration of vehicles at zero speed
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering", meta = (ClampMin = "0.0"))
    float MaxThrottleAcceleration;

    // The maximum braking deceleration of characters, animals and vehicles
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering", meta = (ClampMin = "0.0"))
    float MaxBrakeDeceleration;

    // Deceleration (per unit speed) due to the ground, used for characters and animals
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering", meta = (ClampMin = "0.0"))
    float GroundFriction;

    FMercunaGroundNavigationSteeringParameters()
    {
        bKinematicPathfinding = false;
        MaxSpeed = 600.0f;
        MaxAcceleration = 2000.0f;
        MinTurningRadius = 100.0f;
        IdealTurningRadius = 300.0f;
        MaxThrottleAcceleration = 2000.0f;
        MaxBrakeDeceleration = 2000.0f;
        GroundFriction = 8.0f;
    }
};

//-----------------------------------------------------------------------------

/** A subset of the parameters in FMercunaGroundNavigationSteeringParameters
 * which permit a light-weight updated whilst a move is in progress
 * (see UpdateDynamicSteeringParams).
 * Updating these updates the corresponding parameter in
 * FMercunaGroundNavigationSteeringParameters */
USTRUCT(BlueprintType)
struct FMercunaGroundNavigationDynamicSteeringParameters
{
    GENERATED_USTRUCT_BODY()

    // The maximum speed that the pawn can travel at
    UPROPERTY(BlueprintReadWrite, Category = "Steering", EditAnywhere, meta = (ClampMin = "0.0"))
    float MaxSpeed;

    // The maximum acceleration in all directions for characters and animals
    UPROPERTY(BlueprintReadWrite, Category = "Steering", EditAnywhere, meta = (ClampMin = "0.0"))
    float MaxAcceleration;

    // The maximum braking deceleration, used for characters and vehicles
    UPROPERTY(BlueprintReadWrite, Category = "Steering", EditAnywhere, meta = (ClampMin = "0.0"))
    float MaxBrakingDeceleration;

    // Vehicle power divided by mass, in uu^2 s^-3. Only used for vehicles
    UPROPERTY(BlueprintReadWrite, Category = "Steering", EditAnywhere, meta = (ClampMin = "0.0"))
    float PowerToWeightRatio;

    FMercunaGroundNavigationDynamicSteeringParameters()
    {
        MaxSpeed = 600.0f;
        MaxAcceleration = 2000.0f;
        MaxBrakingDeceleration = 2000.0f;
        PowerToWeightRatio = 0.0f;
    }
};

//-----------------------------------------------------------------------------

/** Configuration parameters used by the ground navigation component that controls movement style */
USTRUCT(BlueprintType)
struct FMercunaGroundNavigationConfiguration
{
    GENERATED_USTRUCT_BODY()

    /** Whether should slow down in order to stop at destination */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style")
    bool StopAtDestination;

    // Minimum time for avoidance. Larger times causes this pawn to 
    // avoid earlier and keep further away. 
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style", meta = (ClampMin = "0.01", ClampMax = "5.0"))
    float MinAvoidanceTime;

    // How should this pawn react to an approaching obstacle when it is stopped
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style")
    EMercunaAvoidanceWhenStationary AvoidanceWhenStationary;
    
    // How strongly the pawn avoids steep slopes. 1 keeps to almost 
    // flat surfaces, 0 traverses near the Max Slope Angle.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SlopePenalty;

    // Multiplier for the estimate of tire traction. Values <1 cause 
    // more cautious turning, >1 cause higher speed turns and possible sliding.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style", meta = (ClampMin = "0.5", ClampMax = "1.5"))
    float TractionEstimate;

    // Minimum distance to look towards along the pawn's path, also used as the 
    // distance to steer towards for GetLastInputVector(). 0 uses the default,
    // Min(AgentDiameter, AgentLength).
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Style", meta = (ClampMin = "0.0"))
    float LookAheadDistance;

    FMercunaGroundNavigationConfiguration()
    {
        StopAtDestination = true;
        MinAvoidanceTime = 1.0f;
        AvoidanceWhenStationary = EMercunaAvoidanceWhenStationary::None;
        SlopePenalty = 0.5f;
        TractionEstimate = 1.0f;
        LookAheadDistance = 0.0f;
    }
};

//-----------------------------------------------------------------------------

/** A component for moving a pawn using the Mercuna ground navigation system */
UCLASS(ClassGroup = Mercuna, meta = (DisplayName = "Mercuna Ground Navigation Component", BlueprintSpawnableComponent, ShortTooltip = "Ground Navigation component"))
class MERCUNA_API UMercunaGroundNavigationComponent : public UMercunaNavigationComponent
{
    GENERATED_UCLASS_BODY()

    /** Delegate for reaching the start of an automatically generated jump */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnJumpStarted, FVector, StartPosition, FVector, EndPosition, FVector, SuggestedLaunchVelocity);

public:
    /** Move toward Destination location
    * @param Destination    The position to move to
    * @param EndDistance    The distance from the destination that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath Whether to move as close as possible to the destination if no complete path to the destination can be found
    * @param EndDirection   Facing direction at destination ((0,0,0) for any)
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AdvancedDisplay = "EndDirection"))
    void MoveToLocation(const FVector& Destination, FVector EndDirection = FVector::ZeroVector, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = false);

    /** Move to multiple destination locations
    * @param Destinations   Array of position to move through in turn
    * @param EndDistance    The distance from the final destination that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath Whether to move as close as possible to the destinations if no path to them can be found
    * @param EndDirection   Facing direction at final destination ((0,0,0) for any)
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AdvancedDisplay = "EndDirection"))
    void MoveToLocations(const TArray<FVector>& Destinations, FVector EndDirection = FVector::ZeroVector, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = true);

    /** Add an additional destination to the end of the current path.  The agent must be moving to a location. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void AddDestinationLocation(FVector NextDestination);

    /** Move toward Actor. While the path will update as the target Actor moves, once Pawn is within range the
    *   action will finish.
    * @param Actor          The target actor to move to
    * @param EndDistance    The distance from the target actor that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath Whether to move as close as possible to the actor if no complete path can be found
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (AdvancedDisplay = "UsePartialPath"))
    void MoveToActor(AActor* Actor, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = false);

    /** Request that the pawn follows another actor */
    virtual FAIRequestID RequestTrackActor(AActor* Actor, float Distance = 0.0f, float Speed = 1.0f, FVector Offset = FVector::ZeroVector, bool bOffsetInWorldSpace = false, bool UsePartialPath = true, bool bAvoidTrackActor = false);

    /** Follow specified Actor trying to stay within a specific distance of the target. This action never completes.
    * @param Actor          The target actor to follow
    * @param Distance       The distance from the target actor that the Pawn tries to stay within
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param Offset         The offset from the tracked actor in the tracked actor's local frame.
    * @param UsePartialPath Whether to continue to track the actor, even if the actor can not be reached.
    */
    UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "Offset, UsePartialPath"), Category = "Mercuna|Ground Navigation")
    void TrackActor(AActor* Actor, float Distance = 0.0f, float Speed = 1.0f, FVector Offset = FVector::ZeroVector, bool UsePartialPath = true);

    /** Bring the Pawn to a complete stop as rapidly as possible. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void Stop();

    /** Request that the Pawn moves to specified goal or actor */
    FAIRequestID RequestMoveTo(const FMercunaMoveRequestGround& MoveRequest);

    /** Request that the Pawn comes to a complete stop as soon as possible */
    virtual FAIRequestID RequestStop() override;

    /** If already doing a MoveTo action update the destination. Returns false if the MoveTo action can't be updated. */
    virtual bool UpdateMoveToLocation(FAIRequestID RequestID, FVector NewDestination) override;

    /** Add an extra destination when running a MoveTo action. Returns false if the MoveTo action can't be updated. */
    virtual bool AddMoveToLocation(FAIRequestID RequestID, FVector NextDestination) override;

    /** Terminate any active movement actions */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void CancelMovement();
    virtual void CancelMovement(FAIRequestID RequestId) override;

    /** Blueprint notification that the pawn has completed the current movement request */
    UPROPERTY(BlueprintAssignable)
    FMercunaMoveComplete OnMoveCompleted;

    /** Blueprint notification that the pawn has started to navigate a manually generated jump link
    * If this is bound, game code is responible for traversing the jump and must call NavLinkComplete once the jump is finished
    */
    UPROPERTY(BlueprintAssignable, Category = "Mercuna|Ground Navigation")
    FOnJumpStarted OnJumpStarted;

    /** Notify that transition of the given nav link is complete */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void NavLinkComplete();

    /** If the pawn is following a path then get the approximate distance to the end of the path
     *   This may return that the path is not valid if it is still being computed.
     */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void GetPathInfo(bool& Valid, float& DistanceToEnd);

    /** Get remaining path length.
     * Returns straight line distance if the path is not yet available.
     * Returns zero if there is no active move request. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    float GetRemainingPathLength();

    /** Get the spline that the pawn is currently following, if any.
      If the pawn is not following a path, or the Smooth Paths configuration option is turned off, 
      then an empty spline is returned. */
    virtual FSplineCurves GetCurrentSpline() const override;

PRAGMA_DISABLE_DEPRECATION_WARNINGS
    /** Get the current trajectory, if any, at rate SampleRate
     * @param TrajectorySampleRange If the return value is Trajectory, TrajectorySampleRange.Samples is set, any other value (None/Avoiding/NavLink) and it is empty
     * @param LookAhead             Time to look ahead
     * @param SampleRate            Minimum number of samples per second (generally equally spaced with some transitions inserted)
     * @param ReturnAbsoluteTimes   Whether returned times are relative (default) or absolute (world)
     */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    EMerNavigationTrajectory GetTrajectory(FTrajectorySampleRange& TrajectorySampleRange, float LookAhead=2.0f, int32 SampleRate = 30, bool ReturnAbsoluteTimes = false) const;
PRAGMA_ENABLE_DEPRECATION_WARNINGS

    /** Implied Input vector for use in animation, comparable to PawnMovementComponent::GetLastInputVector. Not set for vehicles */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    FVector GetLastInputVector() const { return LastInputVector; }

    /** Disable and re-enable avoidance against a particular actor. The actor needs to have a 
    *   MercunaObstacleComponent to be considered as an obstacle in the first place. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    virtual void SetAvoidanceAgainst(AActor* Actor, bool Enable = false) override;

    /** Reset avoidance excluded actors */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    virtual void ClearAvoidanceExclusions() override;

    /** Get the look direction that the current command is requesting, this is a direction vector from the actor's location.
     * Note, this is not valid for vehicles. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    virtual FVector GetLookDirection();

    /** Get the current movement style configuration */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    FORCEINLINE FMercunaGroundNavigationConfiguration GetConfiguration() const { return Configuration; }

    /** Get the current steering parameters */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    FORCEINLINE FMercunaGroundNavigationSteeringParameters GetSteeringParameters() const { return SteeringParameters; }

    /** Reconfigure the navigation component to switch parameters controlling the
     * movement style. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void Configure(const FMercunaGroundNavigationConfiguration& NewConfiguration);

    /** Reconfigure the navigation component to switch parameters controlling steering.
     * Will disable bAutomaticSteeringParameters. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void ConfigureSteering(const FMercunaGroundNavigationSteeringParameters& NewSteering);

    /** Reconfigure the navigation component. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void ConfigureMovement(EMercunaAvoidanceMode NewAvoidanceMode = EMercunaAvoidanceMode::ORCA);

    /** Get the current dynamic steering parameters */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    FMercunaGroundNavigationDynamicSteeringParameters GetDynamicSteeringParameters() const;

    /** Update the dynamic steering parameters without cancelling the current move.
     * These changes persist for subsequent move requests. These can be updated
     * frequently, although not every frame.
     */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void UpdateDynamicSteeringParams(const FMercunaGroundNavigationDynamicSteeringParameters& NewDynamicSteeringParams);

    /** Sets this pawn's AgentType at runtime. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void SetAgentType(FMercunaGroundAgentType NewAgentType);

    /** Set the nav grid that pawn should use */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void SetNavGrid(AMercunaNavGraph2D* NavGrid);

    /** Choose the best available nav grid for this pawn at its current location */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void SetNavGridToBest();

    /** Get the nav grid this pawn is using */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    AMercunaNavGrid* GetNavGrid() const;
    virtual AMercunaNavGraph* GetNavGraph() const override;

    /** Set the usage flags for the pawn */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    virtual void SetUsageFlags(FMercunaPawnUsageFlags NewUsageFlags) override;

    /** Pauses control of this pawn, this should be used when Game logic is taking control of this pawn's movement.
    * This does not clear the navigation command.
    * Navigation is automatically resumed when any move request is made.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void PauseNavigation();

    /** Resume navigation. Used to continue navigating after a call to PauseNavigation.
     */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void ResumeNavigation();

    /** Set the movement component to be used */
    void SetMovementComponent(UPawnMovementComponent* NewMovementComponent);

    //-------------------------------------------------------------------------

    virtual ENavType GetType() const override { return ENavType::Ground; }

    const FMercunaAgentType& GetAgentType() const { return AgentType; }

    virtual void SetNavGraph(AMercunaNavGraph* pNavGraph) override;
    virtual void SetNavGraphToBest() override { SetNavGridToBest(); }

    virtual void Activate(bool bReset) override;
    virtual void Deactivate() override;

    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
    virtual bool HasSuccessfullyInitialized() const override;

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
    friend class FMercunaGroundNavigationComponentVisualizer;

    /** The nav grid for this pawn to use. If not set then the most suitable nav grid will be automatically used */
    UPROPERTY(EditAnywhere, Category = Navigation)
    TObjectPtr<AMercunaNavGraph2D> NavGrid;

    /** The agent type of this pawn from the ones defined in the project settings.
    * The nav grid this pawn is assigned to must support this agent type */
    UPROPERTY(EditAnywhere, Category = Navigation)
    FMercunaAgentType AgentType;

    /** Configuration properties defining the movement style of the pawn */
    UPROPERTY(Category = "Configuration", EditAnywhere, BlueprintReadOnly, meta = (ShowOnlyInnerProperties))
    FMercunaGroundNavigationConfiguration Configuration;

    // Should Mercuna attempt to automatically calculate the steering parameters
    // from the movement component. The derived values can be viewed in the
    // details panel when in PiE mode.
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Navigation)
    bool bAutomaticSteeringParameters;

    /** Parameters that define the steering ability of the pawn */
    UPROPERTY(Category = Navigation, EditAnywhere, BlueprintReadOnly)
    FMercunaGroundNavigationSteeringParameters SteeringParameters;

    /** Whether to set the rotation on the CharacterMovementComponent controller.
    * Normally this would be used with UseControllerRotationYaw=OrientRotationToMovement=false, 
    * UseControllerDesiredRotation=true. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Navigation)
    bool bSetCharacterControllerRotation;

    // Current look direction
    FVector LookDir = FVector::ZeroVector;

    // If traversing a nav link the link being traversed
    TWeakObjectPtr<AMercunaNavLink> CurrentNavLink;

    //-------------------------------------------------------------------------

    // Is this actor currently running dynamic avoidance
    virtual bool DoesAvoidance(AActor* Actor = nullptr) const override;

    virtual FVector GetPathPosOffset() const override;

    virtual void UpdateDebugInfo(float DeltaTime);

    // Find suitable movement component and initialise related state 
    virtual bool InitializeMovementComponent(const EMercunaGroundAgentCategory& AgentCategory);

    // Populate the steeringParams that describe the capabilities of the agent based on its movement component
    virtual void ConfigureMovementSteeringParams(const EMercunaGroundAgentCategory& AgentCategory, const FMercuna2DAgentProperties* AgentProperties, mercuna::MerSteeringParams2D& steeringParams);

    // Called every frame to query the driver for desired movement and pass output to movement component
    virtual void ProcessMovement(float DeltaTime);

    void ProcessVehicleMovement(float DeltaTime);
    void ProcessCharacterMovement(float DeltaTime);

    virtual void SetController(AController* InController) override;

    /** Check that the pawn is in navigable space. If no grid is set then attempt to
    find the best ones to use */
    virtual bool CheckNavigable();

    virtual void OnMoveCompleteDelegate(const FAIRequestID& RequestId, EMercunaMoveResult::Type result) override;

    // Called when nav link traversal is complete - either due to being informed by game code
    // or due to the driver detecting that path following has finished traversing the link
    virtual void EndLinkTraversal();

    /** Find steering for the given angle (and wheel base) */
    static float VehicleSteeringFromAngle(const float turnRightAngle, const float distFromAxelWithoutSteering, const float halfAxelLength, const float distanceBetweenAxels, const float maxWheelAngle);

    /** Construct a MerEntity and return a pointer to it. */
    virtual mercuna::MerNavigatingEntityPtr GetMerEntity() const override;

    /** Get the internal navigator */
    virtual mercuna::MerPtr<mercuna::IMerNavigator> GetNavigator() const override;

    /** Get the internal driver */
    mercuna::MerPtr<mercuna::IMerDriver> GetDriver() const;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;
private:
    friend class MerVehicleController;

    /** The movement component that is driven by this navigation component.
    *   Only one of these should be set at a time */
    class UChaosWheeledVehicleMovementComponent* VehicleMovementComponent;
    class UCharacterMovementComponent* CharacterMovementComponent;

    class MerVehicleController* VehicleController = nullptr;

    void ConfigCharacter(const FMercuna2DAgentProperties* AgentProperties, mercuna::MerSteeringParams2D& steeringParams) const;

    void ConfigureMovementInternal(EMercunaAvoidanceMode NewAvoidanceMode);

    void DeriveSteeringParametersCharacter(bool bKinematicPathfinding, const class UCharacterMovementComponent* CharacterMovementComp,
                                           FMercunaGroundNavigationSteeringParameters& DerivedSteeringParameters) const;

    /** Hides the private Mercuna internal types. Is only created when component is initialized */
    mercuna::MercunaGroundNavigationComponentImpl* m_pImpl;

    /** Last input vector, Velocity / MaxSpeed */
    FVector LastInputVector;
};
