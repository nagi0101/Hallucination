// Copyright (C) 2020 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// An actor context marks that any debug draw performed within its scope
// related to a specific actor.

class MERCUNA_API MercunaActorContext
{
public:
    MercunaActorContext(AActor* Actor);
    ~MercunaActorContext();
};

// A debug context allows sets of debug draw to be grouped together and
// enabled/disabled as one.

class MERCUNA_API MercunaDebugDrawContext
{
public:
    MercunaDebugDrawContext(const char* DebugContextName);
    MercunaDebugDrawContext(const FString& DebugContextName);
    ~MercunaDebugDrawContext();
};

// Test is a specific debug draw context is currently enabled. This can be
// use to avoid doing expensive draw operations if the results are not going
// to be seen.

MERCUNA_API bool IsMercunaDrawContextEnabled(const char* DebugContextName);
MERCUNA_API bool IsMercunaDrawContextEnabled(const FString& DebugContextName);

//-----------------------------------------------------------------------------
// Shapes

// Draw a straight line
MERCUNA_API void MercunaDrawLine(const FVector& Start, const FVector& End, const FColor& Color, const float Thickness = 1.0f);

// Draw a polyline made up of multiple segments
MERCUNA_API void MercunaDrawPloyline(const TArray<FVector>& Points, const FColor& Color, const float Thickness = 1.0f);
 
// Draw an arrow pointing from start to end
MERCUNA_API void MercunaDrawArrow(const FVector& Start, const FVector& End, const FColor& Color, const float MaxHeadLength = 0.0f);

// Draw a sphere
MERCUNA_API void MercunaDrawSphere(const FVector& Center, const float Radius, const FColor& Color);


//-----------------------------------------------------------------------------
// Text

// Draw text on an entity
MERCUNA_API void MercunaDrawEntityText(const FString& Text, const FColor& Color);

// Draw text at 2D screen position
MERCUNA_API void MercunaDraw2DText(const float X, const float Y, const FString& Text, const FColor& Color);
