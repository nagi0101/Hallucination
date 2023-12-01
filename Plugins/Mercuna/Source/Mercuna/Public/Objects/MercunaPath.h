// Copyright (C) 2017-2019 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "MercunaPath.generated.h"

namespace mercuna
{
    struct MercunaPathImpl;
    enum class EMerPathEvent : unsigned int;
}

/**
*   A Mercuna path to either a static location or to an actor which can be moving
*/

UCLASS(BlueprintType)
class MERCUNA_API UMercunaPath : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    // Has the path finished being generated
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    bool IsReady() const;

    // Has a valid path been found - path that are in the process of generating will
    // also be considered invalid until they are Ready
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    bool IsValid() const;

    // Does the path reach the destination or only get part of the way there
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    bool IsPartial() const;

    // The number of points that make up the path. Will always be at least two if the path
    // is valid
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    int GetNumPoints() const;

    // The total length of the path
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    float GetPathLength() const;

    // Get position of point i
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    FVector GetPoint(int i) const;

    // Return description of path status
    FString GetStatus() const;

    // UObject begin
    virtual void BeginDestroy() override;
    // UObject end

    void DebugDraw(FColor color = FColor::White);

    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void GetDebugInfo(int& nodesUsed, bool& bOutOfNodes, float& queryTime) const;

protected:
    friend class AMercunaNavOctree;
    friend class AMercunaNavGrid;

    // Called when the path changes
    UPROPERTY()
    FOnMercunaPathUpdated PathUpdated;

    // Hides internal Mercuna path
    mercuna::MercunaPathImpl* m_pImpl;

    void OnPathUpdate(mercuna::EMerPathEvent PathEvent);
};