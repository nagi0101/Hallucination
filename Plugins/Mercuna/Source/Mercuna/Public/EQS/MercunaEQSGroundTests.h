// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"

#include "MercunaTypes.h"

#include "MercunaEQSGroundTests.generated.h"

namespace mercuna
{
    class IMerNavGraph2D;
    template<typename T> class MerPtr;
}

class UEnvQueryContext;

UCLASS(Abstract)
class MERCUNA_API UMercuna2DQueryTest : public UEnvQueryTest
{
    GENERATED_UCLASS_BODY()

    /** The class of agent to use for determining the test parameters, if None the parameters are taken from the query's owner. */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    TSubclassOf<APawn> AgentClass;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|Ground Navigation", meta = (DisplayName = "Mercuna Ground: Navigable"))
class MERCUNA_API UEnvQueryTest_Mercuna2D_Navigable : public UMercuna2DQueryTest
{
    GENERATED_UCLASS_BODY()

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|Ground Navigation", meta = (DisplayName = "Mercuna Ground: Reachable"))
class MERCUNA_API UEnvQueryTest_Mercuna2D_Reachable : public UMercuna2DQueryTest
{
    GENERATED_UCLASS_BODY()

    /** Approximate maximum path length
    * If set to 0 (default) then a fast test is used with a high path length limit.
    * If a limit is specified, then a slower test must be used to allow the path length to be checked.
    */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    FAIDataProviderFloatValue MaxPathLength;

    /** Clamp distance
    * If set to a positive value, the locations are clamped to a navigable position within
    * the given horizontal distance before testing reachability. By default (-1) no
    * clamping is performed.
    * If the clamp was successful points (i.e. subclasses of UEnvQueryItemType_Point) will
    * be moved to the clamped location (i.e. Actors will remain at their original locations).
    */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    FAIDataProviderFloatValue ClampDistance;

    /** Context to check reachability from. */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    TSubclassOf<UEnvQueryContext> Context;

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|Ground Navigation", meta = (DisplayName = "Mercuna Ground: Path Length"))
class MERCUNA_API UEnvQueryTest_Mercuna2D_PathLength : public UMercuna2DQueryTest
{
    GENERATED_UCLASS_BODY()

    /** The maximum path distance to consider before a point is considered unreachable. */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    FAIDataProviderFloatValue MaxPathDistance;

    /** Clamp distance
    * If set to a positive value, the locations are clamped to a navigable position within
    * the given horizontal distance before testing path distance. By default (-1) no
    * clamping is performed.
    * If the clamp was successful points (i.e. subclasses of UEnvQueryItemType_Point) will
    * be moved to the clamped location (i.e. Actors will remain at their original locations).
    */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    FAIDataProviderFloatValue ClampDistance;

    /** Context to determine path starting location. */
    UPROPERTY(EditDefaultsOnly, Category = Reachable)
    TSubclassOf<UEnvQueryContext> Context;

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

//-----------------------------------------------------------------------------

UCLASS(Category = "Mercuna|Ground Navigation", meta = (DisplayName = "Mercuna Ground: NavRaycast"))
class MERCUNA_API UEnvQueryTest_Mercuna2D_NavRaycast : public UMercuna2DQueryTest
{
    GENERATED_UCLASS_BODY()

    /** Context to raycast from */
    UPROPERTY(EditDefaultsOnly, Category = Raycast)
    TSubclassOf<UEnvQueryContext> Context;

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

//-------------------------------------------------------------------------------

//The direction in which a point can be projected onto the navgrid.
UENUM(BlueprintType)
enum class EMercunaProjectionType : uint8 
{
    Up,
    Down,
    UpOrDown
};

//--------------------------------------------------------------------------------

UCLASS(Category = "Mercuna|Ground Navigation", meta = (DisplayName = "Mercuna Ground: Project"))
class MERCUNA_API UEnvQueryTest_Mercuna2D_Project : public UMercuna2DQueryTest
{
    GENERATED_UCLASS_BODY()

    /* The maximum distance a point can be projected onto the navgrid.
     * If the ItemType is a subclass of UEnvQueryItemType_Point then
     * successfully projected points will be moved to the projected
     * position.
     */
    UPROPERTY(EditDefaultsOnly, Category = Project)
    FAIDataProviderFloatValue MaxProjectionDistance;

    // The direction in which a point can be projected onto the navgrid.
    UPROPERTY(EditDefaultsOnly, Category = Project)
    EMercunaProjectionType ProjectionType;

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};
