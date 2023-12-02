// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Runtime/Launch/Resources/Version.h"
#include "GameFramework/Volume.h"
#include "Other/MerSettings.h"
#include "MercunaNavGraph.h"
#include "MercunaNavVolume.generated.h"

class UMerVolumeComponent;

UENUM()
enum class EMercunaLevelOfDetail : uint8
{
    Full = 0       UMETA(DisplayName = "Full"),
    Half = 1       UMETA(DisplayName = "1/2"),
    Quarter = 2    UMETA(DisplayName = "1/4"),
    Eighth = 3     UMETA(DisplayName = "1/8"),
    Sixteenth = 4  UMETA(DisplayName = "1/16"),
};


/**
* Defines navigation bounds volume in which to generate the Mercuna navigation graphs
*/
UCLASS(Abstract, hidecategories=(Collision, Input, Physics, Networking), NotBlueprintable)
class MERCUNA_API AMercunaNavVolume : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    /** Resize the nav volume at runtime.  Nav graphs must be rebuilt for this to take effect.
    * @param Size           New side length for each side of the box
    */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void SetSize(FVector Size);

    /** Get the unscaled size of the nav volume */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    FVector GetSize() const;

    /** Set the orientation of this volume, its nav graphs and all its associated volumes */
    UFUNCTION(BlueprintCallable, Category = "Mercuna")
    void SetNavigationRotation(const FRotator& Rotation);
    void SetNavigationOrientation(const FQuat& Orientation);

    //-----------------------------------------------------------------------------

    // Is a point inside the volume, taking into account if precise boundaries is off
    bool IsInside(FVector Position) const;

    // Get the nav graph this volume is linked to
    virtual AMercunaNavGraph* GetNavGraph() const { return nullptr; }

    FORCEINLINE bool GetPreciseBoundaries() const { return PreciseBoundaries; }

    // Return the navgraph relative bounds taking into account precise/non-precise boundaries
    FBox GetNavRelativeBounds(const FVector& CellSize, const FIntVector& CellsInGenerationBox, const FTransform& NavGraphTransform) const;

    virtual void PostLoad() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override
    {
        PostDuplicate(DuplicateMode == EDuplicateMode::PIE);
    }
    virtual void PostRegisterAllComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Destroyed() override;
    virtual void BeginDestroy() override;
    virtual bool IsLevelBoundsRelevant() const override { return false; }

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
#else
    virtual void PreEditChange(FProperty* PropertyThatWillChange) override;
#endif
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditMove(bool bFinished) override;
    virtual FName GetNavGraphPropertyName() const { return FName(); }
#endif

protected:
    UPROPERTY(Category = Mercuna, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMerVolumeComponent> VolumeComponent;

    /** Whether to treat the bounds of the nav volume as hard walls.  If this is false then 
     * the volume will be expanded so the edges align well with the nav graph.
     * Turning this off can reduce memory usage significantly, but means pawns will be able 
     * to navigate for some distance outside of the specified bounds. */
    UPROPERTY(EditAnywhere, Category = "Mercuna")
    bool PreciseBoundaries = true;

    // Soft references can't be used in BeginDestroy so also need a pointer to the navgraph
    UPROPERTY(Transient)
    TObjectPtr<AMercunaNavGraph> NavGraphPtr = nullptr;

    // Following property only exists now for migration purposes
    UE_DEPRECATED(2.0, "Deprecated as no longer based on volume base class")
    UPROPERTY()
    TObjectPtr<UBrushComponent> BrushComponent;

    //-------------------------------------------------------------------------

    // Is this nav volume in an editor world
    bool IsEditorMode() const;

    // Is this nav volumes linked to a nav graph
    bool IsLinkedToGraph() const;

    // Link the volume to a graph
    virtual void SetNavGraph(AMercunaNavGraph* NavGraph) {}

    // Add this volume to a new graph
    void AddToGraph(AMercunaNavGraph* NewNavGraph);

    // Remove this volume from its current graph
    void RemoveFromGraph();

    // Find a graph and link with it, if one doesn't exist then create one
    template <typename T>
    void AutoLink(bool bCreateNewGraphIfRequired);

    // Called when this actor is moved or modified
    void OnUpdated();
};

template <typename T>
void AMercunaNavVolume::AutoLink(bool bCreateNewGraphIfRequired)
{
    if (GetDefault<UMerSettings>()->bAutoLinkNavVolumesWithGraphs)
    {
        // Try and find an existing graph in the level
        SetNavGraph(T::GetInstance(GetWorld(), GetLevel()));

        if (!IsLinkedToGraph() && bCreateNewGraphIfRequired)
        {
            // No graph exists create one
            SetNavGraph(T::CreateInstance(GetWorld(), GetLevel()));
        }
    }
}
