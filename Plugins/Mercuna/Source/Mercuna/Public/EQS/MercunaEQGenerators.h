// Copyright (C) 2017 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "DataProviders/AIDataProvider.h"
#include "MercunaEQGenerators.generated.h"

UENUM(BlueprintType)
enum class EMercunaPointDistribution : uint8
{
    //  Points will be uniformly distributed on a shell with equal spacing
    Uniform,

    //  Points will be randomly distributed over the surface of each shell
    Random
};

UCLASS(category = Mercuna, meta = (DisplayName = "Points: Sphere"))
class MERCUNA_API UEnvQueryGenerator_Mercuna_Sphere : public UEnvQueryGenerator
{
    GENERATED_UCLASS_BODY()

    /** min distance between point and context */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue InnerRadius;

    /** max distance between point and context */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue OuterRadius;

    /** number of shells to generate */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderIntValue NumberOfShells;

    /** number of items to generate for each shell */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderIntValue PointsPerShell;

    /** how should the points be distributed */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    EMercunaPointDistribution Distribution;

    /** context */
    UPROPERTY(EditAnywhere, Category = Generator)
    TSubclassOf<class UEnvQueryContext> Center;

    virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};

UCLASS(category = Mercuna, meta = (DisplayName = "Points: 3D Ring"))
class MERCUNA_API UEnvQueryGenerator_Mercuna_3DRing : public UEnvQueryGenerator
{
    GENERATED_UCLASS_BODY()

    /** min distance between point and context */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue InnerRadius;

    /** max distance between point and context */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue OuterRadius;

    /** number of rings to generate */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderIntValue NumberOfRings;

    /** number of items to generate for each ring */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderIntValue PointsPerRing;

    /** min height between point and context */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue MinHeight;

    /** max height between point and context */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue MaxHeight;

    /** number of vertical layer of rings */
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderIntValue NumberOfLayers;

    /** context */
    UPROPERTY(EditAnywhere, Category = Generator)
    TSubclassOf<class UEnvQueryContext> Center;

    virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
};
