// Copyright (C) 2019 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Objects/MercunaPath.h"
#include "Components/SplineComponent.h"

#include "MercunaSmoothPath.generated.h"

/**
*   A Mercuna smooth path to either a static location or to an actor which can be moving
*/

UCLASS(BlueprintType)
class MERCUNA_API UMercunaSmoothPath : public UMercunaPath
{
    GENERATED_UCLASS_BODY()

public:
    // Get the position along the path curve, where 0 <= Length <= path length
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    FVector GetPosition(const float Length) const;

    // Get the tangent along the path curve, where 0 <= Length <= path length
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    FVector GetTangent(const float Length) const;

    // Get the spline points
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    TArray<FSplinePoint> GetSplinePoints(const FTransform& RelativeTo) const;

    // Convert to a spline curve
    FSplineCurves GetSplineCurve() const;
};
