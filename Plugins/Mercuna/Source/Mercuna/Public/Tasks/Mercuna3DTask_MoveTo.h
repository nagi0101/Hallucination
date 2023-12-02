// Copyright (C) 2017-2020 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Tasks/AITask.h"

#include "Components/Mercuna3DNavigationComponent.h"

#include "Mercuna3DTask_MoveTo.generated.h"

UCLASS()
class MERCUNA_API UMercuna3DTask_MoveTo : public UAITask
{
    GENERATED_UCLASS_BODY()

    /** prepare move task for activation */
    void SetUp(AAIController* Controller, const FMercunaMoveRequest3D& InMoveRequest);

    /** Start movement based on current move request */
    virtual void PerformMove();

    FORCEINLINE EMercunaMoveResult::Type GetMoveResult() const { return MoveResult; }
    FORCEINLINE bool WasMoveSuccessful() const { return MoveResult == EMercunaMoveResult::Success; }

    UFUNCTION(BlueprintCallable, Category = "Mercuna|3D Navigation|Tasks", meta = (AdvancedDisplay = "EndDistance,AcceptPartialPath", DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE", DisplayName = "Move To Location or Actor (3D)"))
    static UMercuna3DTask_MoveTo* MoveTo(AAIController* Controller, FVector GoalLocation, AActor* GoalActor = nullptr, float EndDistance = 0.0f, bool AcceptPartialPath = true, bool bLockAILogic = true);

protected:
    UPROPERTY(BlueprintAssignable)
    FGenericGameplayTaskDelegate OnRequestFailed;

    UPROPERTY(BlueprintAssignable)
    FMercunaMoveTaskCompletedSignature OnMoveFinished;

    /** Request ID of the movement request */
    FAIRequestID MoveRequestID;

    /** Current move request */
    FMercunaMoveRequest3D MoveRequest;

    /** Outcome of the last move */
    EMercunaMoveResult::Type MoveResult;

    void FinishMoveTask(EMercunaMoveResult::Type InResult);

    UMercuna3DNavigationComponent* GetNavigationComponent();

    virtual void Activate() override;
    virtual void OnDestroy(bool bOwnerFinished) override;

    /** Remove all delegates */
    virtual void ResetObservers();

    /** Called by navigation component delegate when move it complete */
    UFUNCTION()
    void OnMoveComplete(FAIRequestID RequestID, EMercunaMoveResult::Type Result);
};
