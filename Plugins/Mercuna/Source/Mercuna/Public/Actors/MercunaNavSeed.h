// Copyright (C) 2017-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"

#include "MercunaNavSeed.generated.h"

/** Used to mark which part of a level navigation will happen in */
UCLASS(MinimalAPI)
class AMercunaNavSeed : public AActor
{
    GENERATED_UCLASS_BODY()

#if WITH_EDITORONLY_DATA
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Display, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UBillboardComponent> SpriteComponent;
#endif
};