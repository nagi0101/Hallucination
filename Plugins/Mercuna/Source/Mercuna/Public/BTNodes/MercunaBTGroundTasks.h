// Copyright (C) 2019-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "MercunaBTGroundTasks.generated.h"

class UMercunaGroundTask_MoveTo;
class UBlackboardComponent;
struct FMercunaMoveRequestGround;

struct FBTMercunaGroundMoveToTaskMemory
{
    FDelegateHandle BBObserverDelegateHandle;
    FVector PreviousGoalLocation;

    TWeakObjectPtr<UMercunaGroundTask_MoveTo> Task;

    uint8 bObserverCanFinishTask : 1;
};

/**
* Mercuna Ground Move To task node.
* Moves the AI pawn toward the specified Actor or Location blackboard entry using Mercuna Ground navigation
*/
UCLASS(meta = (DisplayName = "Mercuna Ground Move To"))
class MERCUNA_API UBTTask_MercunaGround_MoveTo : public UBTTask_BlackboardBase
{
    GENERATED_UCLASS_BODY()

    /** What fraction of its top speed should the Pawn aim to move at */
    UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
    float Speed;
    
    /** How far from the goal should the move to task be considered complete */
    UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
    float EndDistance;

    /** if move goal in BB changes the move will be redirected to new location */
    UPROPERTY(Category = Blackboard, EditAnywhere)
    uint32 bObserveBlackboardValue : 1;

    /** if set, use incomplete path when goal can't be reached */
    UPROPERTY(Category = Node, EditAnywhere, AdvancedDisplay)
    uint32 bAllowPartialPath : 1;

    /** If the target is an obstacle then should avoidance be run against it */
    UPROPERTY(Category = Node, EditAnywhere, AdvancedDisplay)
    uint32 bAvoidTargetActor : 1;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
    virtual uint16 GetInstanceMemorySize() const override;

    virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
    EBlackboardNotificationResult OnBlackboardValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID);

    virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
    virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
    virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR

protected:

    EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
    
    /** prepares move task for activation */
    virtual UMercunaGroundTask_MoveTo* PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UMercunaGroundTask_MoveTo* ExistingTask, FMercunaMoveRequestGround& MoveRequest);
};
