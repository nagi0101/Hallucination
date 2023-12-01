// Copyright (C) 2021-2022 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "DataProviders/AIDataProvider.h"
#include "MercunaEQGroundGenerators.generated.h"

UCLASS(Abstract)
class MERCUNA_API UMercuna2DQueryGenerator : public UEnvQueryGenerator
{
    GENERATED_UCLASS_BODY()

    /** The class of agent to use for determining the generator parameters, if None the parameters are taken from the generator's owner. */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    TSubclassOf<APawn> AgentClass;
};

UCLASS(category = Mercuna, meta = (DisplayName = "Points: Random on Nav Grid"))
class MERCUNA_API UEnvQueryGenerator_Mercuna_2DRandom : public UMercuna2DQueryGenerator
{
    GENERATED_UCLASS_BODY()

    /** The maximum path distance from the Centre*/
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue MaxDistance;

    /** The number of points to generate*/
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderIntValue NumberOfPoints;

    /** The QueryContext specifying the centre point of this query */
    UPROPERTY(EditAnywhere, Category = Generator)
    TSubclassOf<class UEnvQueryContext> Centre;

    virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

UCLASS(category = Mercuna, meta = (DisplayName = "Points: Grid on Nav Grid"))
class MERCUNA_API UEnvQueryGenerator_Mercuna_2DNavGrid : public UMercuna2DQueryGenerator
{
    GENERATED_UCLASS_BODY()

    /** The maximum path distance or radius from the Centre*/
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue MaxDistance;

    /** Whether to treat max distance as a path distance or radius*/
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderBoolValue MaxDistanceIsPathDistance;

    /** The spacing between points in the grid*/
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue SpaceBetween;

    /** The QueryContext specifying the centre point of this query */
    UPROPERTY(EditAnywhere, Category = Generator)
    TSubclassOf<class UEnvQueryContext> Centre;

    virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};
