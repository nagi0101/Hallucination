// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "Kismet/BlueprintAsyncActionBase.h"
#include "MercunaOctreeProxy.generated.h"

//-----------------------------------------------------------------------------

UCLASS(MinimalAPI, meta = (DisplayName = "Mercuna 3D Navigation Clamp To Navigable Proxy"))
class UMercunaOctreeClampToNavigableProxy : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UMercunaOctreeClampToNavigableProxy(const FObjectInitializer& ObjectInitializer);

    // Called when an action successfully completes
    UPROPERTY(BlueprintAssignable)
    FMercunaClampToNavigableResult OnSuccess;

    // Called when an action fails
    UPROPERTY(BlueprintAssignable)
    FMercunaClampToNavigableResult OnFailure;

    // Clamp a position to navigable space.
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|3D Navigation")
    static UMercunaOctreeClampToNavigableProxy* ClampToNavigable(UObject* WorldContextObject, FVector Position, float NavigationRadius, float SearchRadius);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

private:
    // The world context object in which this call is taking place
    UPROPERTY(Transient)
    TObjectPtr<UObject> WorldContextObject;

    FVector Position;
    float NavigationRadius;
    float SearchRadius;
};

//-----------------------------------------------------------------------------

UCLASS(MinimalAPI, meta = (DisplayName = "Mercuna 3D Navigation Raycast Proxy"))
class UMercunaOctreeRaycastProxy : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UMercunaOctreeRaycastProxy(const FObjectInitializer& ObjectInitializer);

    // Called when an action successfully completes
    UPROPERTY(BlueprintAssignable)
    FMercunaRaycastResult OnNoHit;

    // Called when an action fails
    UPROPERTY(BlueprintAssignable)
    FMercunaRaycastResult OnHit;

    /** Check whether this is a clear navigable line between start and end for an agent with the
    * specified navigation radius. Returns hit position on failure.*/
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Mercuna|3D Navigation")
    static UMercunaOctreeRaycastProxy* NavigationRaycast(UObject* WorldContextObject, FVector Start, FVector End, float NavigationRadius);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

private:
    // The world context object in which this call is taking place
    UPROPERTY(Transient)
    TObjectPtr<UObject> WorldContextObject;

    FVector Start;
    FVector End;
    float NavigationRadius;
};