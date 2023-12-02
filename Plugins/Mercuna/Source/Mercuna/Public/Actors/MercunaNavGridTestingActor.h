// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Actors/MercunaNavTestingActor.h"

#include "MercunaNavGridTestingActor.generated.h"

UCLASS()
class MERCUNA_API AMercunaNavGridTestingActor : public AMercunaNavTestingActor
{
    GENERATED_UCLASS_BODY()

public:
    // The agent type
    UPROPERTY(EditAnywhere, Category = Pathfinding)
    FMercunaAgentType AgentType;

    // The other end of the test path
    UPROPERTY(EditAnywhere, NonTransactional, Category = Pathfinding)
    TObjectPtr<AMercunaNavGridTestingActor> OtherActor;

    // The minimum turning radius to use
    UPROPERTY(EditAnywhere, Category = Pathfinding, meta = (ClampMin = "0.0"))
    float MinTurnRadius;

    // The ideal turning radius to use
    UPROPERTY(EditAnywhere, Category = Pathfinding, meta = (ClampMin = "0.0"))
    float IdealTurnRadius;

    // The max speed to use
    UPROPERTY(EditAnywhere, Category = Pathfinding, meta = (ClampMin = "0.0"))
    float MaxSpeed;

    // The speed of the agent at the start of the pathfind
    UPROPERTY(EditAnywhere, Category = Pathfinding)
    float InitialSpeed;

    // Slope penalty (0 = no penalty for height change, 1 = max penalty)
    UPROPERTY(EditAnywhere, Category = Pathfinding, AdvancedDisplay, meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SlopePenalty;

    // Create a path matching the direction of the end point
    UPROPERTY(EditAnywhere, Category = Pathfinding, AdvancedDisplay)
    bool bUseEndDirection = false;

    //-------------------------------------------------------------------------

    /** Blueprint notification that the nav testing actor path has changed */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna")
    FMercunaPathUpdated OnPathUpdated;

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual bool CanEditChange(const UProperty* Property) const override;
#else
    virtual bool CanEditChange(const FProperty* Property) const override;
#endif
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

protected:
#if WITH_EDITORONLY_DATA
    UPROPERTY()
    TObjectPtr<class UArrowComponent> ArrowComponent;
#endif

    virtual AMercunaNavTestingActor* GetOtherActor() const override { return OtherActor; }
    virtual void SetOtherActor(AMercunaNavTestingActor* OtherActor) override;

    virtual void ClearPath() override;
    virtual void FindPath() override;
    virtual void BroadcastPathUpdated() override;
};
