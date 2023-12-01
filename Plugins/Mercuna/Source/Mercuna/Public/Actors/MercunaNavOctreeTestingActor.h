// Copyright (C) 2017-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Actors/MercunaNavTestingActor.h"

#include "MercunaNavOctreeTestingActor.generated.h"

UCLASS()
class MERCUNA_API AMercunaNavOctreeTestingActor : public AMercunaNavTestingActor
{
    GENERATED_UCLASS_BODY()

public:
    // The other end of the test path
    UPROPERTY(EditAnywhere, NonTransactional, Category = Pathfinding)
    TObjectPtr<AMercunaNavOctreeTestingActor> OtherActor;

    // The radius of the actor to use when testing paths
    UPROPERTY(EditAnywhere, Category = Pathfinding)
    float Radius;

    // Height change penalty (0 = no penalty for height change, 1 = max penalty)
    UPROPERTY(EditAnywhere, Category = Pathfinding, AdvancedDisplay, meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float HeightChangePenalty;

    // The max speed to use when testing smooth paths
    UPROPERTY(EditAnywhere, Category = "Smooth Path")
    float MaxSpeed;

    // The max acceleration to use when testing smooth paths
    UPROPERTY(EditAnywhere, Category = "Smooth Path")
    float MaxAcceleration;

    //-------------------------------------------------------------------------

    /** Blueprint notification that the nav testing actor path has changed */
    UPROPERTY(NonPIEDuplicateTransient, BlueprintAssignable, Category = "Mercuna")
    FMercunaPathUpdated OnPathUpdated;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

protected:
    virtual AMercunaNavTestingActor* GetOtherActor() const override { return OtherActor; }
    virtual void SetOtherActor(AMercunaNavTestingActor* OtherActor) override;

    virtual void ClearPath() override;
    virtual void FindPath() override;
    virtual void BroadcastPathUpdated() override;
};