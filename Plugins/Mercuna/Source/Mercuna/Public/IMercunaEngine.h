// Copyright (C) 2016-2022 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"

#include "MercunaTypes.h"

namespace mercuna
{
    // Interface for use by other Mercuna modules
    class IMercunaEngine
    {
    public:
        virtual ~IMercunaEngine() {}

        // Set the current Mercuna debug actor. It must have a Mercuna navigation component
        // to be valid, otherwise will just be ignored.
        virtual void SetDebugActor(AActor* actor) = 0;

        // Clear the current Mercuna debug actor
        virtual void ClearDebugActor() = 0;

        // Is the current Mercuna debug actor set to anything
        virtual bool IsDebugActorSet() const = 0;

        //---------------------------------------------------------------------

        // Used to set the current octree debug draw setting for all octrees
        virtual void SetNavOctreeDebugDrawMode(EMerOctreeDebugDrawMode mode) = 0;

        // Get the current global octree debug draw mode
        virtual EMerOctreeDebugDrawMode GetNavOctreeDebugDrawMode() const = 0;

        // Set/get the override radius for octree debug draw
        virtual void SetNavOctreeDebugDrawRadius(float overrideRadius) = 0;
        virtual float GetNavOctreeDebugDrawRadiusOverride() const = 0;

        //---------------------------------------------------------------------

         // Used to set the current nav grid debug draw setting for all grids
        virtual void SetNavGridDebugDrawMode(EMerGridDebugDrawMode mode) = 0;

        // Get the current global grid debug draw mode
        virtual EMerGridDebugDrawMode GetNavGridDebugDrawMode() const = 0;

        // Used to set the current nav grid debug draw setting for all grids
        virtual void SetNavGridDebugDrawSubmode(EMerGridDebugDrawSubmode submode) = 0;

        // Get the current global grid debug draw sub mode
        virtual EMerGridDebugDrawSubmode GetNavGridDebugDrawSubmode() const = 0;

        // Set the current agent type to draw the grid for
        virtual void SetNavGridDebugDrawAgentType(const FMercunaAgentType& AgentType) = 0;

        // Get the current agent type for which the grids are drawing
        virtual const FMercunaAgentType& GetNavGridDebugDrawAgentType() const = 0;
    };
}
