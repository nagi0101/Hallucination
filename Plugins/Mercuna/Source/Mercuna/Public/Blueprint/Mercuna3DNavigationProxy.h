// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AITypes.h"

#include "Mercuna3DNavigationProxy.generated.h"

//-----------------------------------------------------------------------------

UCLASS(MinimalAPI, meta=(DisplayName="Mercuna 3D Navigation Move To Proxy"))
class UMercuna3DMoveToProxy : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UMercuna3DMoveToProxy(const FObjectInitializer& ObjectInitializer);

    // Called when an action successfully completes
    UPROPERTY(BlueprintAssignable)
    FMercunaNavigationResult OnSuccess;

    // Called when an action fails or is interrupted
    UPROPERTY(BlueprintAssignable)
    FMercunaNavigationResult OnFailure;

    // Simple order for Pawn with a Mercuna 3D Navigation component to move to Destination location.
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Move To Location (3D)", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|3D Navigation")
    static UMercuna3DMoveToProxy* MoveToLocation(UObject* WorldContextObject, APawn* Pawn, const FVector& Destination, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = true);

    // Simple order for Pawn with a Mercuna Navigation component to move to a series of Destination locations
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Move To Locations (3D)", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|3D Navigation")
    static UMercuna3DMoveToProxy* MoveToLocations(UObject* WorldContextObject, APawn* Pawn, const TArray<FVector>& Destinations, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = true);

    /** Simple order for Pawn with a Mercuna 3D Navigation component to move towards specified Actor.
    * Once Pawn is within range the action will finish.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Move To Actor (3D)", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|3D Navigation")
    static UMercuna3DMoveToProxy* MoveToActor(UObject* WorldContextObject, APawn* Pawn, AActor* Actor, float EndDistance = 0.0f, float Speed = 1.0f, bool UsePartialPath = true);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

private:
    enum class E3DMoveType 
    {
        MoveToActor,
        MoveToLocation,
        MoveToLocations,
    };

    UPROPERTY(Transient)
    TObjectPtr<APawn> Pawn;

    E3DMoveType MoveType;
    TWeakObjectPtr<AActor> DestinationActor;
    FVector DestinationLocation;
    TArray<FVector> DestinationLocations;

    float EndDistance;
    float Speed;
    bool UsePartialPath;

    // Store the request ID so match it in callback
    FAIRequestID CurrentRequestId = FAIRequestID::InvalidRequest;

    UFUNCTION()
    void OnMoveComplete(FAIRequestID RequestID, EMercunaMoveResult::Type Result);
};

//-----------------------------------------------------------------------------

UCLASS(MinimalAPI, meta = (DisplayName = "Mercuna 3D Navigation Track Proxy"))
class UMercuna3DTrackActorProxy : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UMercuna3DTrackActorProxy(const FObjectInitializer& ObjectInitializer);

    // Called when an action fails or is interrupted
    UPROPERTY(BlueprintAssignable)
    FMercunaNavigationResult OnFailure;

    /** Follow specified Actor trying to stay within a specific distance of the target. This action never completes.
    * @param Actor          The target actor to follow
    * @param Distance       The distance from the target actor that the Pawn tries to stay within
    * @param Offset         The offset from the tracked actor in the tracked actor's local frame.
    * @param Speed          The speed that the Pawn should try and move at relative to its normal movement speed
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation", meta = (DisplayName = "Track Actor (3D)", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AdvancedDisplay = "Offset"))
    static UMercuna3DTrackActorProxy* TrackActor(UObject* WorldContextObject, APawn* Pawn, AActor* Actor, float Distance, float Speed = 1.0f, FVector Offset = FVector::ZeroVector);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

private:
    UPROPERTY(Transient)
    TObjectPtr<APawn> Pawn;

    TWeakObjectPtr<AActor> DestinationActor;

    float Distance;
    FVector Offset;
    float Speed;

    // Store the request ID so match it in callback
    FAIRequestID CurrentRequestId = FAIRequestID::InvalidRequest;

    UFUNCTION()
    void OnMoveComplete(FAIRequestID RequestID, EMercunaMoveResult::Type Result);
};
