// Copyright (C) 2020-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MercunaPhysicalMaterial.generated.h"

/**
 * A physical material with additional properties used by Mercuna for navigation
 */
UCLASS()
class MERCUNA_API UMercunaPhysicalMaterial : public UPhysicalMaterial
{
    GENERATED_BODY()

public:

    /** How much extra does it cost to navigate on this material compared to normal */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Navigation, meta=(ClampMin=1.0, ClampMax=15.0))
    float CostMultipler = 1.0f;
};
