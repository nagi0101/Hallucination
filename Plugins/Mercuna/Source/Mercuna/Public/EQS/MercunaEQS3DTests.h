// Copyright (C) 2016-2017 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"

#include "MercunaTypes.h"

#include "MercunaEQS3DTests.generated.h"

namespace mercuna
{
    class IMerNavGraph3D;
    template<typename T> class MerPtr;
}

//#define USE_ASYNC_EQS_TEST

class UEnvQueryContext;
#ifndef USE_ASYNC_EQS_TEST
struct FEnvQueryTaskData {};
#endif

UCLASS(Abstract)
class MERCUNA_API UMercuna3DQueryTest : public UEnvQueryTest
{
    GENERATED_UCLASS_BODY()

protected:
    mercuna::MerPtr<mercuna::IMerNavGraph3D> GetOctreeAndRadius(FEnvQueryInstance& QueryInstance, float& Radius, FMercunaPawnUsageFlags& UsageFlags) const;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|3D Navigation", meta = (DisplayName = "Mercuna 3D: Navigable Volume"))
class MERCUNA_API UEnvQueryTest_Mercuna3D_NavigableVolume : public UMercuna3DQueryTest
{
    GENERATED_UCLASS_BODY()

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|3D Navigation", meta = (DisplayName = "Mercuna 3D: Reachable"))
class MERCUNA_API UEnvQueryTest_Mercuna3D_Reachable : public UMercuna3DQueryTest
{
    GENERATED_UCLASS_BODY()

    /** Approximate maximum path length
    * If set to 0 (default) then a fast test is used with a high path length limit.
    * If a limit is specified, then a slower test must be used to allow the path length to be checked.
    */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    FAIDataProviderFloatValue MaxPathLength;

    /** Context to check reachability from. */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    TSubclassOf<UEnvQueryContext> Context;

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
    virtual void ContinueAfterTaskComplete(FEnvQueryInstance& QueryInstance, FEnvQueryTaskData* TaskData) const
#ifdef USE_ASYNC_EQS_TEST
        override;
#else
        ;
#endif

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|3D Navigation", meta = (DisplayName = "Mercuna 3D: NavRaycast"))
class MERCUNA_API UEnvQueryTest_Mercuna3D_NavRaycast : public UMercuna3DQueryTest
{
    GENERATED_UCLASS_BODY()

    /** Context to raycast from */
    UPROPERTY(EditDefaultsOnly, Category = Raycast)
    TSubclassOf<UEnvQueryContext> Context;

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|3D Navigation", meta = (DisplayName = "Mercuna 3D: Project"))
class MERCUNA_API UEnvQueryTest_Mercuna3D_Project : public UMercuna3DQueryTest
{
    GENERATED_UCLASS_BODY()

    /** Maximum distance to move items to clamp them to navigable space
    * If the value is negative then defaults to a radius based on octree cell size and navigation radius
    */
    UPROPERTY(EditDefaultsOnly, Category = Project)
    FAIDataProviderFloatValue MaxSearchRadius;

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};
