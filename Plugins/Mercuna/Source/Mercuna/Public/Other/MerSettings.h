// Copyright (C) 2016-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.h"

#include "Runtime/Launch/Resources/Version.h"

#include "MerSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class MERCUNA_API UMerSettings : public UObject
{
    GENERATED_UCLASS_BODY()

    DECLARE_MULTICAST_DELEGATE(FOnMerSettingsChanged);

public:

    //-----------------------------------------------------------------------------
    // Global Mercuna debug settings
    //-----------------------------------------------------------------------------
    
    // A length suitable for debug draw
    UPROPERTY(EditAnywhere, config, Category = Debug)
    float DebugLengthScale;

    // Whether errors relating to pawn configuration should always be displayed 
    // on screen, even when the General debug draw category is disabled.
    //
    UPROPERTY(EditAnywhere, config, Category = Debug)
    bool bAlwaysShowErrors;

    //-----------------------------------------------------------------------------
    // Global Mercuna configuration settings
    //-----------------------------------------------------------------------------

    /** Define the usage types that can be applied to Mercuna Nav Modifier Volumes.
    * Pawns must then be configured via their Mercuna Navigation components as to
    *  whether they can enter volumes with specific usage types set.
    */
    UPROPERTY(EditAnywhere, Category = Configuration, config)
    TArray<FName> ModifierUsageTypes;

    /** Defines the set of agent types that use ground navigation and the properties
    * for each type. Pawns must then be configured via their Mercuna Ground Navigation
    * components as to which agent type they are and this determines which navigation
    * subgrid they use.
    */
    UPROPERTY(EditAnywhere, Category = Configuration, config)
    TMap<FName, FMercunaGroundAgentProperties> GroundAgentTypes;

    /** When enabled assumes that there will be only one nav graph of each type per 
    * level (or sublevel), so automatically links newly created nav volumes with the 
    * first nav graph found, or creates a new nav graph if one is not already present 
    * in the level.
    */
    UPROPERTY(EditAnywhere, Category = Configuration, config)
    bool bAutoLinkNavVolumesWithGraphs;

    /** If set, then when a level is loaded at runtime, nav graphs of the same type, can merge
     * with each other to allow continuous navigation between the graphs. Merging can only
     * happen when both graphs are set to allow merging and have the same configuration parameters. 
    */
    UPROPERTY(EditAnywhere, Category = "Defaults", config)
    bool bAllowNavGraphMerging;

    /** The main thread CPU time limit per frame (in seconds) for collecting world geometry (physics meshes)
     * when building nav graphs.
     */
    UPROPERTY(EditAnywhere, Category = Configuration, config, meta = (ClampMin = "0.001", ClampMax = "0.05"))
    double GeometryCollectionTimePerFrame;

    /** When running in single threaded mode (usually on dedicated servers) the CPU time limit per frame
     * (in seconds) for running jobs.
    */
    UPROPERTY(EditAnywhere, Category = Configuration, config, meta = (ClampMin = "0.005", ClampMax = "0.1"))
    float SingleThreadedJobTimePerFrame;

    //-----------------------------------------------------------------------------
    // NavOctree generation parameters
    //-----------------------------------------------------------------------------

    /** Determines the side length of the cubes that make up the lowest level of the octree.
    * Cells are considered unnavigable if there is any level geometry within them, so the
    * larger the cell size the greater the error margin in the representation of the geometry.
    */
    UPROPERTY(EditAnywhere, Category = "NavOctree Defaults", config, meta = (ClampMin = "1.0"))
    float OctreeCellSize;

    /** The minimum pawn radius determine what navigation data is stored in the octree.
    * Paths will never go closer to geometry than the minimum pawn radius
    */
    UPROPERTY(EditAnywhere, Category = "NavOctree Defaults", config, meta = (ClampMin = "1", ClampMax = "6"))
    int32 MinPawnRadius;

    /** The maximum pawn radius determine what navigation data is stored in the octree.
    * The octree doesn't store data to allow paths to be found with more clearance from
    * geometry than the maximum radius.
    */
    UPROPERTY(EditAnywhere, Category = "NavOctree Defaults", config, meta = (ClampMin = "1", ClampMax = "24"))
    int32 MaxPawnRadius;

    //-----------------------------------------------------------------------------
    // NavGrid generation parameters
    //-----------------------------------------------------------------------------

    /** Normally, the agent step height is added to the height clearance to determine the minimum vertical
     * space required for navigation.  If this option is set to true, then the step height is not added.
     * WARNING: Enabling this option can result in space being marked as navigable when there is not in fact enough vertical
     * height for the agent to pass through, in the case that there is a step very close to a low doorway or within a low tunnel.
     * NOTE: You must rebuild all of your nav grids after changing this setting. */
    UPROPERTY(EditAnywhere, Category = "NavGrid", config)
    bool bIgnoreStepForHeightClearance;

    //-----------------------------------------------------------------------------

    const FMercuna2DAgentProperties* GetAgentProperties(const FMercunaAgentType& AgentType) const;

    //-----------------------------------------------------------------------------

    /** Gets the multicast delegate which is run whenever the settings changes. */
    FOnMerSettingsChanged& GetOnMerSettingsChanged();

    void TransferSettings() const;

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual bool CanEditChange(const UProperty* Property) const override;
#else
    virtual bool CanEditChange(const FProperty* Property) const override;
#endif
    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    virtual void PostInitProperties() override;

private:
    FOnMerSettingsChanged OnMerSettingsChanged;
};

//-----------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FMerOctreeEditorSettings
{
    GENERATED_USTRUCT_BODY()

    // Color of the navigable cells in the octree debug draw 
    UPROPERTY(EditAnywhere, config, Category = "Nav Octree Debug")
    FLinearColor NavigableColor;

    // Color of high cost navigable cells in the octree debug draw 
    UPROPERTY(EditAnywhere, config, Category = "Nav Octree Debug")
    FLinearColor HighCostColor;

    // Color of the unnavigable cells in the octree debug draw 
    UPROPERTY(EditAnywhere, config, Category = "Nav Octree Debug")
    FLinearColor UnnavigableColor;

    /** Defaults */
    FMerOctreeEditorSettings()
    {
        NavigableColor = FLinearColor(0.0f, 0.6f, 0.0f, 0.8f);
        HighCostColor = FLinearColor(1.0f, 0.25f, 0.025f, 1.0f);
        UnnavigableColor = FLinearColor(0.5f, 0.0f, 0.0f, 1.0f);
    }
};

//-----------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FMerGridEditorSettings
{
    GENERATED_USTRUCT_BODY()

    // Color of the navigable cells in the nav grid debug draw 
    UPROPERTY(EditAnywhere, config, Category = "Nav Grid Debug")
    FLinearColor NavigableColor;

    // Color of the edges of auto-generated jump links
    UPROPERTY(EditAnywhere, config, Category = "Nav Grid Debug")
    FLinearColor AutoJumpLinkColor;

    // Height above geometry to draw nav grid. If the ground clips through
    // the grid this may need to be increased
    UPROPERTY(EditAnywhere, config, Category = "Nav Grid Debug")
    float VerticalOffset;

    // How think to draw the lines between nav grid cells
    UPROPERTY(EditAnywhere, config, Category = "Nav Grid Debug", meta = (ClampMin = "1.0", ClampMax = "20.0"))
    float LineThickness;

    /** Defaults */
    FMerGridEditorSettings()
    {
        NavigableColor = FLinearColor(0.0f, 0.6f, 0.0f, 0.8f);
        AutoJumpLinkColor = FLinearColor(0.0f, 1.0f, 2.0f, 1.0f);
        VerticalOffset = 20.0f;
        LineThickness = 4.0f;
    }
};

//-----------------------------------------------------------------------------

UCLASS(config = EditorPerProjectUserSettings)
class MERCUNA_API UMerEditorSettings : public UObject
{
    GENERATED_UCLASS_BODY()

    DECLARE_MULTICAST_DELEGATE(FOnMerEditorSettingsChanged);

public:
    // When enabled all Mercuna log messages, including debug log messages, are
    // output to a separate Mercuna.log file
    UPROPERTY(EditAnywhere, config, Category = "General Debug")
    bool bEnableExtraLogging;

    // Flushing the Mercuna log file after every write will impact performance
    // but can be useful to ensure that all log messages have been written to
    // the file in event of a crash
    UPROPERTY(EditAnywhere, config, Category = "General Debug", AdvancedDisplay, meta = (EditCondition = "bEnableExtraLogging"))
    bool bFlushMercunaLogFileOnWrite;

    // Settings relating to octree debug draw
    UPROPERTY(Category = "Nav Octree Debug", EditAnywhere, config, meta = (ShowOnlyInnerProperties))
    FMerOctreeEditorSettings OctreeEditorSettings;

    // Settings relating to grid debug draw
    UPROPERTY(Category = "Nav Grid Debug", EditAnywhere, config, meta = (ShowOnlyInnerProperties))
    FMerGridEditorSettings GridEditorSettings;

    /** Gets the multicast delegate which is run whenever the settings changes. */
    FOnMerEditorSettingsChanged& GetOnMerEditorSettingsChanged();

    void TransferSettings() const;

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
    virtual bool CanEditChange(const UProperty* Property) const override;
#else
    virtual bool CanEditChange(const FProperty* Property) const override;
#endif
    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    FOnMerEditorSettingsChanged OnMerEditorSettingsChanged;
};
