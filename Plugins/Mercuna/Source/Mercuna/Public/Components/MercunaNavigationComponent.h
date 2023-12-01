// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "AITypes.h"
#include "Components/SplineComponent.h"
#include "Engine/LatentActionManager.h"

#include "MercunaNavigationComponent.generated.h"

// Forward declarations
namespace mercuna
{
    class IMerNavigator;
    template<typename T> class MerPtr;
    class IMerNavigatingEntity;
    template<typename T, size_t IMPLEMENTATION_SIZE> class MerIntegrationPtr;
    typedef MerIntegrationPtr<IMerNavigatingEntity, 32> MerNavigatingEntityPtr;
    class MerObstacle;
}
class AMercunaNavGraph;
class UMerEntitySubsystem;
class FGameplayDebuggerCategory_Mercuna;

//-----------------------------------------------------------------------------

struct FMercunaContextualSteeringActorParameters
{
    // These must mirror MerContextInfo::ContextVersusAgentParams
    float RepulsionDistance = 500.0f;   // Avoid going closer than this to other agent
    float RepulsionWeight = 1.0f;       // 0 indicates unused
    float CohesionDistance = 1000.0f;   // Avoid getting farther than this from other agent. When used, should be >= repulsionDistance
    float CohesionWeight = 0.0f;        // 0 indicates unused
};

//-----------------------------------------------------------------------------

/** An abstract component that serves as the base class for other navigation components */
UCLASS(Abstract, ClassGroup = Mercuna, NotBlueprintable, hideCategories = (Trigger, PhysicsVolume, Collision), meta = (BlueprintSpawnableComponent))
class MERCUNA_API UMercunaNavigationComponent : public UActorComponent
{
    GENERATED_UCLASS_BODY()

    /** Delegate for requesting contextual steering parameters from game code */
    DECLARE_DELEGATE_TwoParams(FGetContextualSteeringParams, AActor*, FMercunaContextualSteeringActorParameters&);

public:
    enum class ENavType
    {
        None,
        ThreeDimensional,
        Ground
    };

    enum class EState
    {
        Inactive,
        Idle,
        Stopping,
        MovingToLocation,
        MovingToActor,
        TrackingActor
    };

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMercunaMoveComplete, FAIRequestID, RequestID, const EMercunaMoveResult::Type, Result);

    virtual ENavType GetType() const { return ENavType::None; }

    // Get the nav graph this pawn is using
    virtual AMercunaNavGraph* GetNavGraph() const { return nullptr; }

    // Set the nav graph this pawn is using, the nav graph must be of the correct type for the component.
    virtual void SetNavGraph(AMercunaNavGraph* pNavGraph) { checkNoEntry(); }

    // Choose the best available nav graph for this pawn at its current location.
    virtual void SetNavGraphToBest() { checkNoEntry(); }

    // Is this actor currently running dynamic avoidance
    // If an actor (that is an obstacle) is specified then checks whether that particular 
    // actor is avoided
    virtual bool DoesAvoidance(AActor* Actor = nullptr) const;

    /** Disable and re-enable avoidance against a particular actor. The actor needs to have a 
    *   MercunaObstacleComponent to be considered as an obstacle in the first place. */
    virtual void SetAvoidanceAgainst(AActor* Actor, bool Enable = false) { checkNoEntry(); }

    /** Reset avoidance excluded actors */
    virtual void ClearAvoidanceExclusions() { checkNoEntry(); }

    /** Set the usage flags for the pawn */
    virtual void SetUsageFlags(FMercunaPawnUsageFlags NewUsageFlags);

    /** Get the spline that the pawn is currently following, if any.
      If the pawn is not following a path, or the Smooth Paths configuration option is turned off, 
      then an empty spline is returned. */
    virtual FSplineCurves GetCurrentSpline() const { return FSplineCurves(); }

    // Get the current requested destination, returns false if none
    bool GetRequestedDestination(FVector& Location) const;

    /** Request that the Pawn comes to a complete stop as soon as possible */
    virtual FAIRequestID RequestStop() { return FAIRequestID::InvalidRequest; }

    /** If already doing a MoveTo action update the destination. Returns false if the MoveTo action can't be updated. */
    virtual bool UpdateMoveToLocation(FAIRequestID RequestID, FVector NewDestination) { return false; }

    /** Add an extra destination when running a MoveTo action. Returns false if the MoveTo action can't be updated. */
    virtual bool AddMoveToLocation(FAIRequestID RequestID, FVector NextDestination) { return false; }

    /** If tracking another actor update the offset. The offset is in the same frame as the original request
    * @param NewOffset          The offset to the target actor
    */
    virtual void UpdateTrackOffset(const FVector& NewOffset);

    /** Override the desired move speed for the current move request as a multiplier of the
     * maximum speed. This override only applies to the current request.
     * Returns false if not possible (e.g. for a wheeled vehicle or if there is no move request).
     */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    bool OverrideSpeedMultiplier(const float NewSpeedMultiplier);

    /** Get the currently active movement request ID */
    virtual FAIRequestID GetCurrentRequestId() { return CurrentRequestId; }

    /** Terminate any movement action corresponding to request id */
    virtual void CancelMovement(FAIRequestID RequestId) {}

    FORCEINLINE const FMercunaPawnUsageFlags& GetUsageFlags() const { return UsageFlags; }

    FORCEINLINE EMercunaAvoidanceMode GetAvoidanceMode() const { return AvoidanceMode; }

    /** Was the component successfully setup during initialize */
    virtual bool HasSuccessfullyInitialized() const { checkNoEntry(); return false; }

    /** Return a string describing the last configuration error */
    FORCEINLINE FString GetLastConfigurationError() const { return m_lastError; }

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    //-------------------------------------------------------------------------

    /** Clear all the cached contextual steering parameters for this agent */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void InvalidateContextualSteeringParams();

    /** Clear all the cached contextual steering parameters between this agent and the specified actor. */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void InvalidateContextualSteeringParamsAgainstActor(AActor* Actor);

    /** Callback to game code to get the parameters this agent should use for contextual steering
     * against the specified actor. Parameters will be cached until they are aged out or explicitly
     * invalidated. */
    FGetContextualSteeringParams OnGetContextualSteeringParams;

    // Handle deprecated DynamicAvoidance parameter
    virtual void PostLoad() override;

protected:
    friend mercuna::MerObstacle;
    friend UMerEntitySubsystem;
    friend FGameplayDebuggerCategory_Mercuna;

    static uint32 NextRequestId;
    FAIRequestID CurrentRequestId;

    /** What method to use for interacting with other Mercuna obstacles */
    UPROPERTY(EditAnywhere, Category = Navigation)
    EMercunaAvoidanceMode AvoidanceMode;

    /** Deprecated in favor of AvoidanceMode: Whether to do dynamic collision avoidance against other Mercuna obstacles */
    UPROPERTY(meta = (DeprecatedProperty, DeprecationMessage = "From v2.6 use AvoidanceMode instead."))
    bool DynamicAvoidance;

    /** Multiplier for velocity obstacle weights. A large weight will cause this agent to avoid less, and 
     * neighbors to avoid more. */
    UPROPERTY(EditAnywhere, Category = Navigation, meta = (ClampMin = "0.01", ClampMax = "100.0"))
    float AvoidanceWeightMultiplier = 1.0f;

    /** Determines what type of navigation modifier volumes this
    *   pawn is allowed to or is required to go through. */
    UPROPERTY(EditAnywhere, Category = Navigation)
    FMercunaPawnUsageFlags UsageFlags;

    /** Last navigation error as a string */
    FString m_lastError;

    /** Whether navigation was disabled by a pause request. */
    bool m_bPaused;

    /** What the navigation component is currently doing */
    EState m_state;

    /** Used to buffer move complete callbacks so that they can be called at the end of a tick */
    TArray<TFunction<void()>> m_moveCompleteCallbacks;

    /** Track the current controller of the pawn */
    UPROPERTY(Transient)
    AController* CurrentController = nullptr;

    /** Get the actor this component is navigating */
    AActor* GetNavigatedActor() const { return GetOwner(); }

    /** Callback made to the component when any movement is completed */
    virtual void MoveComplete(FAIRequestID RequestId, EMercunaMoveResult::Type result);

    void DispatchMoveCompleteCallbacks();

    // Called on derived class so they can call their delegates
    virtual void OnMoveCompleteDelegate(const FAIRequestID& RequestId, EMercunaMoveResult::Type result) {}

    // Called when the controller changes (Not called on versions earlier than UE4.26)
    virtual void SetController(AController* InController);

    // Get the cached internal entity
    virtual mercuna::MerNavigatingEntityPtr GetMerEntity() const;

    // For use by MerObstacle to directly access the navigator
    virtual mercuna::MerPtr<mercuna::IMerNavigator> GetNavigator() const;

    // Get the local offset from the actor's position to the position to be used for pathfinding
    virtual FVector GetPathPosOffset() const { return FVector(0.0); }

    // Match the tick interval (esp. of a movement component)
    void MatchTickInterval(const UActorComponent* Component);

private:
    void OnPawnControllerChanged(AController* InController);
};
