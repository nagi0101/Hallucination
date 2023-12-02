// Copyright (C) 2020-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"
#include "GameFramework/Actor.h"

#include "Actors/MercunaNavGrid.h"

#include "MercunaNavLink.generated.h"

class AMercunaNavGraph2D;
class UMerNavLinkRenderingComponent;
class UBillboardComponent;

/** Forms a link between two disconnected parts of the nav grid. Usually used
* for ladders and jumps */
UCLASS()
class MERCUNA_API AMercunaNavLink : public AActor
{
    GENERATED_UCLASS_BODY()
    
public:    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMercunaNavLinkStart, APawn*, Pawn, EMercunaNavigationLinkDirection, Direction, FVector, Destination);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NavLink, meta = (ShowOnlyInnerProperties))
    FMercunaNavigationLink Link;

    // What types of agents can use this link
    UPROPERTY(EditAnywhere, Category = NavLink)
    FMercunaSupportedAgentTypes SupportedAgentTypes;

    // Is the link currently being traversed by an agent
    UFUNCTION(BlueprintCallable, Category="Mercuna|NavLink")
    bool InUse() const { return bInUse; }

    // Change whether a navlink is enabled
    UFUNCTION(BlueprintCallable, Category = "Mercuna|NavLink")
    void SetEnabled(bool bEnabled);

    // Get link left end world position
    UFUNCTION(BlueprintCallable, Category = "Mercuna|NavLink")
    FVector GetLeftPosition() const;

    // Get link right end world position
    UFUNCTION(BlueprintCallable, Category = "Mercuna|NavLink")
    FVector GetRightPosition() const;

    // Get the desired entry rotation for the left end
    UFUNCTION(BlueprintCallable, Category = "Mercuna|NavLink")
    FRotator GetLeftRotation() const;

    // Get the desired entry rotation for the right end
    UFUNCTION(BlueprintCallable, Category = "Mercuna|NavLink")
    FRotator GetRightRotation() const;

    // Get the transform for the left end
    UFUNCTION(BlueprintCallable, Category = "Mercuna|NavLink")
    FTransform GetLeftTransform() const;

    // Get the transform for the right end
    UFUNCTION(BlueprintCallable, Category = "Mercuna|NavLink")
    FTransform GetRightTransform() const;

    FORCEINLINE AMercunaNavGraph2D* GetNavGrid() const { return NavGrid; }
    FORCEINLINE bool AppliesToAllNavGrids() const { return bApplyToAllNavGrids; }
    FORCEINLINE bool IsLinkEnabled() const { return bEnabled; }

    /** Blueprint notification that an actor needs to transition the nav link */
    UPROPERTY(BlueprintAssignable)
    FMercunaNavLinkStart OnNavLinkStart;

    UFUNCTION(BlueprintImplementableEvent)
    void ReceiveNavLinkStart(APawn* Pawn, EMercunaNavigationLinkDirection Direction, FVector Destination);

    UFUNCTION(BlueprintImplementableEvent)
    void ReceiveNavLinkEnd(APawn* Pawn);

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual FBox GetComponentsBoundingBox(bool bNonColliding = false) const override;
#else
    virtual FBox GetComponentsBoundingBox(bool bNonColliding = false, bool bIncludeFromChildActors = false) const override;
#endif

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual bool CanEditChange(const UProperty* Property) const override;
#else
    virtual bool CanEditChange(const FProperty* Property) const override;
#endif
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

    FORCEINLINE const FMercunaNavUsageTypes& GetUsageTypes() const { return UsageTypes; }

private:
    friend class UMercunaGroundNavigationComponent;

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    TObjectPtr<UMerNavLinkRenderingComponent> EdRenderComp;

    UPROPERTY()
    TObjectPtr<UBillboardComponent> SpriteComponent;
#endif // WITH_EDITORONLY_DATA

    /** Whether this nav link should apply to all navigation graphs. */
    UPROPERTY(EditAnywhere, Category = NavLink)
    bool bApplyToAllNavGrids;

    /** Which navigation grid this nav link is associated with. */
    UPROPERTY(EditAnywhere, Category = NavLink, meta=(EditCondition="!bApplyToAllNavGrids"))
    TObjectPtr<AMercunaNavGraph2D> NavGrid;

    /** Set of flags that mark which usage types this nav link requires. */
    UPROPERTY(EditAnywhere, Category = NavLink, AdvancedDisplay)
    FMercunaNavUsageTypes UsageTypes;

    /** Is this link usable */
    UPROPERTY(EditAnywhere, Category = NavLink)
    bool bEnabled;

    // Is the link currently in use
    bool bInUse = false;

    // Called when the link is used by a pawn
    void StartLinkTraversal(APawn* Pawn, EMercunaNavigationLinkDirection Direction);
    void EndLinkTraversal(APawn* Pawn);
};
