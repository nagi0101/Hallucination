// Copyright (C) 2021-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Actors/MercunaNavGrid.h"

#include "MercunaGroundNavInvokerComponent.generated.h"

/** A component for registering a pawn as a Mercuna ground navigation invoker */
UCLASS(ClassGroup=Mercuna, meta = (DisplayName = "Mercuna Ground Navigation Invoker Component", BlueprintSpawnableComponent) )
class MERCUNA_API UMercunaGroundNavInvokerComponent : public UActorComponent
{
    GENERATED_UCLASS_BODY()
public:
    // In Automatic mode the invoker will use the grid and agent type from
    // the MercunaGroundNavigation component if the actor one.
    UPROPERTY(EditAnywhere, Category = "Generation")
    bool bAutomatic;

    // The nav grid this invoker applies to
    UPROPERTY(EditAnywhere, Category = "Generation", meta=(EditCondition="!bAutomatic"))
    TObjectPtr<AMercunaNavGrid> NavGrid;

    // The Agent Types this invoker applies to
    UPROPERTY(EditAnywhere, Category = "Generation", meta = (EditCondition = "!bAutomatic"))
    FMercunaSupportedAgentTypes AgentTypes;

    // The radius within which nav grid will be generated
    UPROPERTY(EditAnywhere, Category = "Generation", meta = (ClampMin = "1.0"))
    float GenerationRadius = 3000.0f;

    // The radius beyond which nav grid will be removed
    UPROPERTY(EditAnywhere, Category = "Generation", meta = (ClampMin = "1.0"))
    float RemovalRadius = 5000.0f;

    // Modify the nav grid at runtime.  This is called automatically by the
    // navigation component when the nav grid is set on the navigation component.
    void SetNavGrid(AMercunaNavGrid* pNavGrid);

    virtual void Activate(bool bReset) override;
    virtual void Deactivate() override;

protected:
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void RegisterInvoker();
};
