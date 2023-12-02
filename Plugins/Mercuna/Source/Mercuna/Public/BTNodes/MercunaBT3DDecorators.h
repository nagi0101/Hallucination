// Copyright (C) 2017-2020 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "MercunaBT3DDecorators.generated.h"

/**
* IsReachable decorator node.
* A decorator node that bases its condition on whether a point is reach from an actor.
*/
UCLASS(meta = (DisplayName = "Mercuna 3D Is Reachable"))
class MERCUNA_API UBTDecorator_Mercuna3D_Reachable : public UBTDecorator
{
    GENERATED_UCLASS_BODY()

protected:

    /** The actor that is the start point */
    UPROPERTY(EditAnywhere, Category = Condition)
    FBlackboardKeySelector Source;

    /** The destination point the actor is trying to reach */
    UPROPERTY(EditAnywhere, Category = Condition)
    FBlackboardKeySelector Destination;

    /** Approximate maximum path length - this is not a strict criteria
    * and is used only to limit the search space. */
    UPROPERTY(Category = Condition, EditAnywhere)
    float MaxPathLength;

public:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
    virtual FString GetStaticDescription() const override;
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

#if WITH_EDITOR
    virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR
};
