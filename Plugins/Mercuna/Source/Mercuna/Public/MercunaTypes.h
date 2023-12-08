// Copyright (C) 2018-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "MercunaTypes.generated.h"

//-----------------------------------------------------------------------------

UENUM()
enum class EMerOctreeDebugDrawMode : uint8
{
    DISABLED        UMETA(DisplayName = "Disabled"),
    UNNAVIGABLE     UMETA(DisplayName = "Draw unnavigable space"),
    NAVIGABLE       UMETA(DisplayName = "Draw navigable space"),
    CROSS_SECTION   UMETA(DisplayName = "Draw slice of unnavigable and navigable space"),
    REACHABLE       UMETA(DisplayName = "Draw cells included in the last reachability query"),
    PATHFIND        UMETA(DisplayName = "Draw cells included in the last pathfind"),
    REGION          UMETA(DisplayName = "Draw navigable space coloured by region"),
};

UENUM()
enum class EMerGridDebugDrawMode : uint8
{
    // Needs to be in same order as EDebugDrawGridMode
    DISABLED        UMETA(DisplayName = "Disabled"),
    GRID            UMETA(DisplayName = "Draw the navigable grid"),
    POLYMESH        UMETA(DisplayName = "Draw the pathfinding polymesh"),
    QUERY           UMETA(DisplayName = "Draw the last search query of the debug actor")
};

UENUM()
enum class EMerGridDebugDrawSubmode : uint8
{
    NAVIGABLE               UMETA(DisplayName = "Draw navigable space"),
    COST                    UMETA(DisplayName = "Draw cost"),
    SURFACE_TYPE            UMETA(DisplayName = "Draw surface type"),
    USAGE_TYPE              UMETA(DisplayName = "Draw usage type"),
    ORIENTATIONS            UMETA(DisplayName = "Draw orientations"),
    NAVLINKS                UMETA(DisplayName = "Draw nav links"),
    SURFACE_NORMALS         UMETA(DisplayName = "Draw surface normals"),
    POLYGONS                UMETA(DisplayName = "Draw polygon ids"),
    CONNECTIONS             UMETA(DisplayName = "Draw connections"),

    QUERY_COST              UMETA(DisplayName = "Draw last query cost"),
    QUERY_HEURISTIC         UMETA(DisplayName = "Draw last query heuristic"),
    QUERY_TOTAL             UMETA(DisplayName = "Draw last query cost + heuristic"),
    QUERY_DISTANCE_FIELD    UMETA(DisplayName = "Draw distance field of last kinematic search")
};

//-----------------------------------------------------------------------------

enum EMerJobType
{
    PATH_FIND,
};

//-----------------------------------------------------------------------------

// Needs to be in same order as MerSteeringParams2D::EAvoidanceMode
UENUM()
enum class EMercunaAvoidanceMode : uint8
{
    // Don't do dynamic avoidance
    None               UMETA(DisplayName = "None"),

    // Use ORCA method for avoidance
    ORCA               UMETA(DisplayName = "ORCA"),

    // Use contextual steering method for avoidance
    ContextualSteering UMETA(DisplayName = "Context Steering"),
};

// Needs to be in same order as MerSteeringParams2D::EAvoidanceWhenStationary
UENUM()
enum class EMercunaAvoidanceWhenStationary : uint8
{
    // Never move out the way
    None               UMETA(DisplayName = "None"),

    // Only move enough to avoid a collision
    CollisionOnly      UMETA(DisplayName = "Collision Only"),

    // Move to avoid collisions and also meet all other constraints
    Full               UMETA(DisplayName = "Full"),
};

// Needs to be in same order as IMerNavigator::TrajectoryState
UENUM(BlueprintType)
enum class EMerNavigationTrajectory : uint8
{
    // Moving along a smooth trajectory (avoidance not required)
    Trajectory         UMETA(DisplayName = "Trajectory"),
    // Motion directed by avoidance
    Avoiding           UMETA(DisplayName = "Avoiding"),
    // On a nav-link
    NavLink            UMETA(DisplayName = "NavLink"),
    // Avoidance is not engaged and no active move request
    None               UMETA(DisplayName = "None"),
};

//-----------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct MERCUNA_API FMercunaNavUsageTypes
{
    GENERATED_USTRUCT_BODY()

#if CPP
    union
    {
        struct
        {
#endif
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType0 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType1 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType2 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType3 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType4 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType5 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType6 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType7 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType8 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType9 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType10 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType11 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType12 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType13 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType14 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType15 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType16 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType17 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType18 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType19 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType20 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType21 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType22 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType23 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType24 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType25 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType26 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType27 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType28 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType29 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType30 : 1;
            UPROPERTY(EditAnywhere, Category = Default)
            uint32 bUsageType31 : 1;
#if CPP
        };
        uint32 PackedBits;
    };
#endif

    FMercunaNavUsageTypes() : PackedBits(0x0)
    {
    }

    bool Serialize(FArchive& Ar)
    {
        Ar << PackedBits;
        return true;
    }
};

template<>
struct TStructOpsTypeTraits< FMercunaNavUsageTypes > : public TStructOpsTypeTraitsBase2< FMercunaNavUsageTypes >
{
    enum
    {
        WithSerializer = true,
    };
};

//-----------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct MERCUNA_API FMercunaPawnUsageFlags
{
    GENERATED_USTRUCT_BODY()

    // If set, pawn can only move in volumes with these usage types
    UPROPERTY(EditAnywhere, Category = Default)
    FMercunaNavUsageTypes RequiredUsageFlags;

    // If set, pawn may enter volumes with these usage types
    UPROPERTY(EditAnywhere, Category = Default)
    FMercunaNavUsageTypes AllowedUsageFlags;

    FMercunaPawnUsageFlags()
    {}

    bool Serialize(FArchive& Ar)
    {
        RequiredUsageFlags.Serialize(Ar);
        AllowedUsageFlags.Serialize(Ar);
        return true;
    }
};

template<>
struct TStructOpsTypeTraits< FMercunaPawnUsageFlags > : public TStructOpsTypeTraitsBase2< FMercunaPawnUsageFlags >
{
    enum
    {
        WithSerializer = true,
    };
};

#define MER_ENGINE_USAGE_TYPE           FMercunaPawnUsageFlags

//-----------------------------------------------------------------------------

UENUM()
namespace EMercunaMoveResult
{
    enum Type
    {
        Success,
        Failed,
        Cancelled,
        Blocked,
        Invalid
    };
}

UENUM()
namespace EMercunaMoveGoal
{
    enum Type
    {
        Actor,
        Vector,
        None
    };
}

UENUM()
namespace EMercunaPathEvent
{
    enum Type
    {
        Ready,
        Updated,
        Invalid
    };
}

USTRUCT()
struct MERCUNA_API FMercunaMoveRequest
{
    GENERATED_USTRUCT_BODY()

    FMercunaMoveRequest()
        : GoalType(EMercunaMoveGoal::Type::None)
        , GoalActor(nullptr)
        , EndDistance(0.0f)
        , Speed(1.0f)
        , bUsePartialPath(true)
        , bAvoidGoalActor(false)
        , StartDirection(FVector::ZeroVector)
    {
    }

    FMercunaMoveRequest(const AActor* InGoalActor, float InEndDistance, float InSpeed, bool bInUsePartialPath, bool bAvoidGoalActor, const FVector& InitialDirection)
        : GoalType(EMercunaMoveGoal::Type::Actor)
        , GoalActor(const_cast<AActor*>(InGoalActor))
        , EndDistance(InEndDistance)
        , Speed(InSpeed)
        , bUsePartialPath(bInUsePartialPath)
        , bAvoidGoalActor(bAvoidGoalActor)
        , StartDirection(InitialDirection)
    {
    }

    FMercunaMoveRequest(const FVector& InGoalLocation, float InEndDistance, float InSpeed, bool bInUsePartialPath, const FVector& InitialDirection)
        : GoalType(EMercunaMoveGoal::Type::Vector)
        , GoalActor(nullptr)
        , GoalLocation(InGoalLocation)
        , EndDistance(InEndDistance)
        , Speed(InSpeed)
        , bUsePartialPath(bInUsePartialPath)
        , bAvoidGoalActor(false)
        , StartDirection(InitialDirection)
    {
    }

    // What type of goal is this move request to
    EMercunaMoveGoal::Type GoalType;

    // The actor to move to
    UPROPERTY()
    TObjectPtr<AActor> GoalActor;

    // The location to move to
    FVector GoalLocation;

    // Via waypoints.  Only valid if the destination is a position.
    TArray<FVector> WaypointLocations;

    // How far from the goal is the move considered complete
    float EndDistance;

    // What relative speed should the agent move at
    float Speed;

    // If there is only a partial path available should it be used
    bool bUsePartialPath;

    // If the goal is an actor should it be avoided
    bool bAvoidGoalActor;

    // Initial direction of the agent (zero indicates any)
    FVector StartDirection;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMercunaPathUpdated, EMercunaPathEvent::Type, PathEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercunaNavigationResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercunaClampToNavigableResult, FVector, ClampedPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercunaRaycastResult, FVector, HitPosition);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMercunaMoveTaskCompletedSignature, EMercunaMoveResult::Type, Result, AAIController*, AIController);

//-----------------------------------------------------------------------------

UENUM()
enum class EMercunaGridType
{
    None,
    Ground,
    Surface,
    Both
};

USTRUCT()
struct FMercunaAgentType
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, Category = AgentType)
    FName Name;

    UPROPERTY(EditAnywhere, Category = AgentType)
    EMercunaGridType GridType;

    FMercunaAgentType() : GridType(EMercunaGridType::Ground) {} // Default grid type to ground so that old (ground) agent types still work
    FMercunaAgentType(const EMercunaGridType& InGridType, const FName& InName) : Name(InName), GridType(InGridType) {}

    bool IsNone() const { return Name.IsNone(); }
    FString GetNameString() const { return Name.GetPlainNameString(); }

    bool operator==(const FMercunaAgentType& OtherAgentType) const { return GridType == OtherAgentType.GridType && Name == OtherAgentType.Name; }

    bool SerializeFromMismatchedTag(struct FPropertyTag const& Tag, FArchive& Ar);

protected:
    FMercunaAgentType(const EMercunaGridType& InGridType) : GridType(InGridType) {}
};

template<>
struct TStructOpsTypeTraits<FMercunaAgentType>
    : public TStructOpsTypeTraitsBase2<FMercunaAgentType>
{
    enum
    {
        WithSerializeFromMismatchedTag = true,
    };
};

USTRUCT(BlueprintType)
struct MERCUNA_API FMercunaGroundAgentType : public FMercunaAgentType
{
    GENERATED_USTRUCT_BODY()

    FMercunaGroundAgentType() : FMercunaAgentType(EMercunaGridType::Ground) {}
    explicit FMercunaGroundAgentType(const FMercunaAgentType& InType) : FMercunaAgentType(EMercunaGridType::Ground, InType.Name) {}
    explicit FMercunaGroundAgentType(const FName& InName) : FMercunaAgentType(EMercunaGridType::Ground, InName) {}
};

inline uint32 GetTypeHash(const FMercunaAgentType AgentType) { return GetTypeHash(AgentType.Name); }

//-----------------------------------------------------------------------------

UENUM()
enum class EMercunaGroundAgentCategory : uint8
{
    Character         UMETA(DisplayName = "Character"),
    Animal            UMETA(DisplayName = "Animal"),
    Vehicle           UMETA(DisplayName = "Vehicle"),
};

UENUM()
enum class EMercunaGroundAgentShape : uint8
{
    Rectangle         UMETA(DisplayName = "Rectangle"),
    Circle           UMETA(DisplayName = "Circle"),
};

UENUM()
enum class EMercunaTurnInPlace : uint8
{
    Never          UMETA(DisplayName = "Can't turn in place"),
    PreferNotTo    UMETA(DisplayName = "Prefers not to turn in place"),
    Allowed        UMETA(DisplayName = "Normally turns in place"),
};

USTRUCT()
struct MERCUNA_API FMercuna2DAgentProperties
{
    GENERATED_USTRUCT_BODY()

    /** The maximum height difference between adjacent cells that is considered traversable, must be less than Height Clearance */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0"))
    float StepHeight;

    /** Minimum free height above geometry for it to be considered navigable
     * (this should be set to the maximum height of any pawn that will navigate on a grid) */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0"))
    float HeightClearance;

    /** Which physical materials is navigation allowed on. If no materials are
    * specified then by default navigation is allowed on any surface regardless 
    * of its physical material (or lack of one). */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (AllowedClasses = "/Script/PhysicsCore.PhysicalMaterial"))
    TSet<struct FSoftObjectPath> NavigableMaterials;

    /** Which physical materials is navigation not allowed. These are ignored if 
    * any materials have been set as NavigableMaterials. */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (AllowedClasses = "/Script/PhysicsCore.PhysicalMaterial"))
    TSet<struct FSoftObjectPath> UnnavigableMaterials;

    FMercuna2DAgentProperties()
    {
        StepHeight = 40.0f;
        HeightClearance = 180.0f;
    }
    virtual ~FMercuna2DAgentProperties() {}

    virtual EMercunaGroundAgentCategory GetCategory() const { return EMercunaGroundAgentCategory::Character; }
    virtual EMercunaGroundAgentShape GetShape() const { return EMercunaGroundAgentShape::Circle; }
    virtual float GetPawnRadius() const { checkNoEntry(); return 0.0f; }
    virtual float GetPawnWidth() const { checkNoEntry(); return 0.0f; }
    virtual float GetPawnLength() const { checkNoEntry(); return 0.0f; }
    virtual float GetMaxLaunchSpeed() const { checkNoEntry(); return 0.0f; }
};

USTRUCT(BlueprintType)
struct MERCUNA_API FMercunaGroundAgentProperties : public FMercuna2DAgentProperties
{
    GENERATED_USTRUCT_BODY()

    /** Determines how the agent does pathfinding. Characters will use normal A* pathfinding
    *   and make sharp turns. Animals use kinematic pathfinding and make gentle turns when 
    *   possible. Vehicles also use kinematic pathfinding but will only make gentle turns. */
    UPROPERTY(EditAnywhere, Category = Default, config)
    EMercunaGroundAgentCategory Category;

    /** For the purposes of navigation on a grid should this pawn be treated as a circle or a rectangle.
    * If a circle then the pawn length is ignored and the pawn width is used as the diameter. */
    UPROPERTY(EditAnywhere, Category = Default, config)
    EMercunaGroundAgentShape Shape;

    /** The width of the largest pawn that will navigate on a grid. */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0"))
    float PawnWidth;

    /** The length of the largest pawn that will navigate on a grid. */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0"))
    float PawnLength;

    /** Maximum angle of a slope that will be considered navigable. Characters can have a
     * maximum angle up to 60 degrees, while vehicles and animals are limited to 45 degrees. */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0", ClampMax = "60.0"))
    float MaxSlopeAngle;

    /** Maximum slope angle change that can be considered navigable */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0", ClampMax = "90.0"))
    float MaxAngleChange;

    /** Maximum speed agent can take off at when jumping */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0"))
    float MaxLaunchSpeed;

    /** Maximum speed agent can land on a surface when jumping */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0"))
    float MaxImpactSpeed;

    /** Minimum vertical launch angle above horizontal when jumping
     *  A minimum of 40, for example, means both launch and landing angle must be above 40 degrees
     *  This is useful to avoid automatic jump links with flat trajectories */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "35.0", ClampMax = "80.0"))
    float MinLaunchAngle;

    /** Maximum vertical angle of either the launch or landing angle (whichever is smaller) above horizontal
     *  A maximum of 60, for example, means that either the launch or landing angle must be below 60 degrees
     *  This is useful to avoid automatic jump links where both the launch and landing are at very steep angles,
     *  such as high jumps over a gap where start and end are at similar heights. */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "35.0", ClampMax = "89.0"))
    float MaxLaunchOrLandAngle;

    /** Maximum horizontal launch angle relative to an edge's perpendicular
     *  A perpendicular launch angle close to 90 allows almost all jumps outwards from an edge
     *  A perpendicular launch angle close to 0 excludes any jumps not perpendicular to an edge */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0", ClampMax = "85.0"))
    float MaxPerpendicularLaunchAngle;

    /** Weight given to automatically generated jumps by path finding, determining how likely a path is to use a jump
     *  A cost of 1.0 always uses a jump if doing so shortens a path; higher than 1.0 discourages paths from using jumps. */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "1.0"))
    float JumpCostMultiplier;

    /** Distance to remain from ledges/precipices, expressed as a fraction of the agent half-width. 
    * Non-circular agents will be set to 1. */
    UPROPERTY(EditAnywhere, Category = Default, config, meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float LedgeMarginFraction;

    FMercunaGroundAgentProperties()
    {
        Category = EMercunaGroundAgentCategory::Character;
        Shape = EMercunaGroundAgentShape::Circle;
        PawnWidth = 70.0;
        PawnLength = 70.0f;
        MaxSlopeAngle = 40.0f;
        MaxAngleChange = 90.f;
        MaxLaunchSpeed = 600.0f;
        MaxImpactSpeed = 850.0f;
        MinLaunchAngle = 35.0f; // Default arctan(CharacterMovementComponent JumpZVelocity/MaxWalkSpeed)
        MaxLaunchOrLandAngle = 89.0f;
        MaxPerpendicularLaunchAngle = 80.0f; // Relative to edge, default most forwards jumps
        JumpCostMultiplier = 1.25f;
        LedgeMarginFraction = 1.0f;
    }

    virtual EMercunaGroundAgentCategory GetCategory() const override { return Category; }
    virtual EMercunaGroundAgentShape GetShape() const override { return Shape; }
    virtual float GetPawnRadius() const override { check(Shape == EMercunaGroundAgentShape::Circle); return PawnWidth * 0.5f; }
    virtual float GetPawnWidth() const override { return PawnWidth; }
    virtual float GetPawnLength() const override { return PawnLength; }
    virtual float GetMaxLaunchSpeed() const override { return MaxLaunchSpeed; }

    // Whether to erode areas of high friction when building the nav grid, derived from the agent category.
    bool ErodeHighFrictionSurfaces() const
    {
        return Category == EMercunaGroundAgentCategory::Vehicle;
    }

    // Whether the agent can reverse
    bool CanReverse() const
    {
        return Category == EMercunaGroundAgentCategory::Vehicle;
    }
};

//-----------------------------------------------------------------------------

UENUM(BlueprintType)
enum class EMercunaNavigationLinkDirection : uint8
{
    BothWays,
    LeftToRight,
    RightToLeft,
};

USTRUCT(BlueprintType)
struct MERCUNA_API FMercunaNavigationLink
{
    GENERATED_USTRUCT_BODY()

    // The position of the left end of the link
    UPROPERTY(EditAnywhere, Category=NavLink, BlueprintReadWrite, meta=(MakeEditWidget=true))
    FTransform Left;

    // The position of the right end of the link
    UPROPERTY(EditAnywhere, Category=NavLink, BlueprintReadWrite, meta=(MakeEditWidget=true))
    FTransform Right;

    // Can this link be traversed in both directions or only one way
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite)
    EMercunaNavigationLinkDirection Direction;

    // Whether to use the distance between the left and right position as the cost of traversing the link.
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite)
    bool bUseDistanceAsCost;

    // Cost of traversing the link from left to right.  When finding a path the link will be considered
    // to be as expensive as following an alternative path to the destination that is this distance.
    // This field is only used in UseDistanceAsCost is unticked.
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite, meta = (ClampMin = 0.0f, EditCondition = "!bUseDistanceAsCost"))
    float LeftToRightCost;

    // Cost of traversing the link from right to left.  When finding a path the link will be considered
    // to be as expensive as following an alternative path to the destination that is this distance.
    // This field is only used in UseDistanceAsCost is unticked.
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite, meta = (ClampMin = 0.0f, EditCondition = "!bUseDistanceAsCost"))
    float RightToLeftCost;

    // If set greater than zero, then when the pawn traversing this link gets with this distance
    // of the left end the link will automatically consider itself to be complete
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite, meta=(ClampMin=0.0f))
    float LeftAutoCompletionDistance;

    // If set greater than zero, then when the pawn traversing this link gets with this distance
    // of the right end the link will automatically consider itself to be complete
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite, meta=(ClampMin=0.0f))
    float RightAutoCompletionDistance;

    // At what angle (in degrees) relative to the link end direction can the left end of the link be entered. 
    // A value of 180 means that the link end can be entered from any direction
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite, meta = (ClampMin = 0.0f, ClampMax = 180.0f))
    float LeftEntryAngle;

    // At what angle (in degrees) relative to the link end direction can the right end of the link be entered. 
    // A value of 180 means that the link end can be entered from any direction
    UPROPERTY(EditAnywhere, Category = NavLink, BlueprintReadWrite, meta = (ClampMin = 0.0f, ClampMax = 180.0f))
    float RightEntryAngle;

    FMercunaNavigationLink()
        : Left(FQuat(0.0f, 0.0f, 1.0f, 0.0f), FVector(-50.0f,0.0f, 0.0f)), Right(FVector(50.0f, 0.0f, 0.0f))
        , Direction(EMercunaNavigationLinkDirection::BothWays)
        , bUseDistanceAsCost(true)
        , LeftToRightCost(100.f)
        , RightToLeftCost(100.f)
        , LeftAutoCompletionDistance(0.0f)
        , RightAutoCompletionDistance(0.0f)
        , LeftEntryAngle(180.0f)
        , RightEntryAngle(180.0f)
    {}

    // Get link left end relative position
    FVector GetLeftPosition() const
    {
        return Left.GetTranslation();
    }

    // Get link right end relative position
    FVector GetRightPosition() const
    {
        return Right.GetTranslation();
    }

    float GetLeftToRightCost() const
    {
        if (bUseDistanceAsCost) return FVector::Distance(Left.GetTranslation(), Right.GetTranslation());
        else return LeftToRightCost;
    }

    float GetRightToLeftCost() const
    {
        if (bUseDistanceAsCost) return FVector::Distance(Left.GetTranslation(), Right.GetTranslation());
        else return RightToLeftCost;
    }
};

//-----------------------------------------------------------------------------

UENUM()
enum class EMercunaProjectionDirection : uint8
{
    Down             UMETA(DisplayName = "Down"),
    Up               UMETA(DisplayName = "Up"),
    Closest         UMETA(DisplayName = "Closest (Up or Down)"),
};

//-----------------------------------------------------------------------------
