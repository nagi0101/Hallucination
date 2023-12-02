// Copyright (C) 2021-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MercunaNavInvokerVolume.generated.h"

class AMercunaNavGrid;
class UMerVolumeComponent;

UENUM()
enum class EMercunaInvokerVolumeType : uint8
{
    // Always generate navgrid inside this volume
    Always,

    // Generate navgrid in the volume only when overlapped by an invoker
    OnOverlap,  
};

/**
 * Volume inside which nav grid can be forced to be generated when
 * using nav invokers
 */
UCLASS(hidecategories = (Collision, Input, Physics, Networking))
class MERCUNA_API AMercunaNavInvokerVolume : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    /** Resize the nav invoker volume at runtime.
    * @param Size  New side length for each side of the box
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void SetSize(FVector Size);

    /** Get the unscaled size of the nav invoker volume */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    FVector GetSize() const;

    /** Add the invoker volume to a nav grid at runtime
*   @param NavGrid  The navigation grid to add the invoker volume to
*/
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void AddToGrid(AMercunaNavGrid* NavGrid);

    /** Set the invoker volume type at runtime */
    UFUNCTION(BlueprintCallable, Category = "Mercuna|Ground Navigation")
    void SetType(const EMercunaInvokerVolumeType& Type);

    bool IsAssociated(AMercunaNavGrid* NavGrid) const;
    EMercunaInvokerVolumeType GetType() const { return Type; }

    virtual void PostActorCreated() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    UPROPERTY(Category = Mercuna, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMerVolumeComponent> VolumeComponent;

    /** Which navigation grid this invoker volume is associated with. */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    TObjectPtr<AMercunaNavGrid> NavGrid;

    /** When should navgrid be generated inside this volume */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    EMercunaInvokerVolumeType Type;
};
