// Copyright (C) 2020-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MercunaBlueprintLibrary.generated.h"

/**
 * Mercuna utility functions
 */
UCLASS()
class MERCUNA_API UMercunaBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // Set the current Mercuna debug actor. It must have a Mercuna navigation component
    // to be valid, otherwise will just be ignored.
    UFUNCTION(BlueprintCallable, Category = "Mercuna", meta=(DevelopmentOnly))
    static void SetDebugActor(AActor* Actor);

    // Clear the current Mercuna debug actor
    UFUNCTION(BlueprintCallable, Category = "Mercuna", meta = (DevelopmentOnly))
    static void ClearDebugActor();

    // Write to the Mercuna log
    // @param Message    String to record to the log
    UFUNCTION(BlueprintCallable, Category = "Mercuna", meta = (DevelopmentOnly))
    static void MercunaLog(const FString& Message);
};
