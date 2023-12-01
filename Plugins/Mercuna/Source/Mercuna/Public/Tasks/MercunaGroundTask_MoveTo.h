// Copyright (C) 2019-2020 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Tasks/AITask.h"

#include "Components/MercunaGroundNavigationComponent.h"

#include "MercunaGroundTask_MoveTo.generated.h"

UCLASS()
class MERCUNA_API UMercunaGroundTask_MoveTo : public UAITask
{
    GENERATED_UCLASS_BODY()

    /** prepare move task for activation */
    void SetUp(AAIController* Controller, const FMercunaMoveRequestGround& InMoveRequest);

    /** Start movement based on current move request */
    virtual void PerformMove();

    FORCEINLINE EMercunaMoveResult::Type GetMoveResult() const { return MoveResult; }
    FORCEINLINE bool WasMoveSuccessful() const { return MoveResult == EMercunaMoveResult::Success; }

    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation|Tasks", meta = (AdvancedDisplay = "EndDistance,AcceptPartialPath", DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE", DisplayName = "Move To Location or Actor (Ground)"))
    static UMercunaGroundTask_MoveTo* MoveTo(AAIController* Controller, FVector GoalLocation, AActor* GoalActor = nullptr, float EndDistance = 0.0f, bool AcceptPartialPath = true, bool bLockAILogic = true);

protected:
    UPROPERTY(BlueprintAssignable)
    FGenericGameplayTaskDelegate OnRequestFailed;

    UPROPERTY(BlueprintAssignable)
    FMercunaMoveTaskCompletedSignature OnMoveFinished;

    /** Request ID of the movement request */
    FAIRequestID MoveRequestID;

    /** Current move request */
    FMercunaMoveRequestGround MoveRequest;

    /** Outcome of the last move */
    EMercunaMoveResult::Type MoveResult;

    void FinishMoveTask(EMercunaMoveResult::Type InResult);

    UMercunaGroundNavigationComponent* GetNavigationComponent();

    virtual void Activate() override;
    virtual void OnDestroy(bool bOwnerFinished) override;

    /** Remove all delegates */
    virtual void ResetObservers();

    /** Called by navigation component delegate when move it complete */
    UFUNCTION()
    void OnMoveComplete(FAIRequestID RequestID, EMercunaMoveResult::Type Result);
};
