// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AITypes.h"

#include "MercunaGroundMoveToProxy.generated.h"

//-----------------------------------------------------------------------------

UCLASS(MinimalAPI, meta = (DisplayName = "Mercuna Ground Navigation Move To Proxy"))
class UMercunaGroundMoveToProxy : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UMercunaGroundMoveToProxy(const FObjectInitializer& ObjectInitializer);

    // Called when an action successfully completes
    UPROPERTY(BlueprintAssignable)
    FMercunaNavigationResult OnSuccess;

    // Called when an action fails or is interrupted
    UPROPERTY(BlueprintAssignable)
    FMercunaNavigationResult OnFailure;

    /** Simple order for Pawn with a Mercuna Ground Navigation component to move to Destination location.
    * @param Pawn           Pawn with a Mercuna Ground Navigation component to move
    * @param Destination    The position to move to
    * @param EndDistance    The distance from the destination that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath Whether to move as close as possible to the destination if no complete path to the destination can be found
    * @param EndDirection   Facing direction at destination((0, 0, 0) for any)
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Move To Location (Ground)", AdvancedDisplay = "EndDirection", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|Ground Navigation")
    static UMercunaGroundMoveToProxy* MoveToLocation(UObject* WorldContextObject, APawn* Pawn, const FVector& Destination, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = false, FVector EndDirection = FVector::ZeroVector);

    /** Simple order for Pawn with a Mercuna Ground Navigation component to move to a series of Destination locations.
    * @param Pawn           Pawn with a Mercuna Ground Navigation component to move
    * @param Destinations   Array of destination locations to move to in order
    * @param EndDistance    The distance from the final destination that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath Whether to move as close as possible to the destinations if no complete path to them can be found
    * @param EndDirection   Facing direction at final destination((0, 0, 0) for any)
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Move To Locations (Ground)", AdvancedDisplay = "EndDirection", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|Ground Navigation")
    static UMercunaGroundMoveToProxy* MoveToLocations(UObject* WorldContextObject, APawn* Pawn, const TArray<FVector>& Destinations, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = false, FVector EndDirection = FVector::ZeroVector);

    /** Simple order for Pawn with a Mercuna Ground Navigation component to move towards specified
    * Actor. Once Pawn is within range the action will finish.
    * @param Pawn           Pawn with a Mercuna Ground Navigation component to move
    * @param Actor          The target actor to move to
    * @param EndDistance    The distance from the target actor that the move is considered complete
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param UsePartialPath Whether to move as close as possible to the actor if no complete path can be found
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Move To Actor (Ground)", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|Ground Navigation")
    static UMercunaGroundMoveToProxy* MoveToActor(UObject* WorldContextObject, APawn* Pawn, AActor* Actor, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = false);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

private:
    enum class EGroundMoveType 
    {
        MoveToActor,
        MoveToLocation,
        MoveToLocations,
    };

    UPROPERTY(Transient)
    TObjectPtr<APawn> Pawn;

    EGroundMoveType MoveType;
    TWeakObjectPtr<AActor> DestinationActor;
    FVector DestinationLocation;
    TArray<FVector> DestinationLocations;

    float EndDistance;
    float Speed;
    bool UsePartialPath;
    FVector EndDirection;

    // Store the request ID so match it in callback
    FAIRequestID CurrentRequestId = FAIRequestID::InvalidRequest;

    UFUNCTION()
    void OnMoveComplete(FAIRequestID RequestID, EMercunaMoveResult::Type Result);
};

//-----------------------------------------------------------------------------

UCLASS(MinimalAPI, meta = (DisplayName = "Mercuna Ground Navigation Track Actor Proxy"))
class UMercunaGroundTrackActorProxy : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UMercunaGroundTrackActorProxy(const FObjectInitializer& ObjectInitializer);

    // Called when an action fails or is interrupted
    UPROPERTY(BlueprintAssignable)
    FMercunaNavigationResult OnFailure;

    /** Follow specified Actor trying to stay within a specific distance of the target. This action never completes.
    * @param Actor          The target actor to follow
    * @param Distance       The distance from the target actor that the Pawn tries to stay within
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    * @param Offset         The offset from the tracked actor in the tracked actor's local frame.
    * @param UsePartialPath Whether to continue to track the actor, even if the actor can not be reached.
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation", meta = (DisplayName = "Track Actor (Ground)", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AdvancedDisplay = "Offset, UsePartialPath"))
    static UMercunaGroundTrackActorProxy* TrackActor(UObject* WorldContextObject, APawn* Pawn, AActor* Actor, float Distance, float Speed = 1.0f, FVector Offset = FVector::ZeroVector, bool UsePartialPath = true);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

private:
    UPROPERTY(Transient)
    TObjectPtr<APawn> Pawn;

    TWeakObjectPtr<AActor> DestinationActor;

    float Distance;
    float Speed;
    FVector Offset;
    bool UsePartialPath;

    // Store the request ID so match it in callback
    FAIRequestID CurrentRequestId = FAIRequestID::InvalidRequest;

    UFUNCTION()
    void OnMoveComplete(FAIRequestID RequestID, EMercunaMoveResult::Type Result);
};
