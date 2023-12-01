// Copyright (C) 2021 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"

#include "Modules/ModuleManager.h"

// Interface for use by other Mercuna modules
class IMercunaModule : public IModuleInterface
{
public:
    // Set the Mercuna log filename, this renames the currently open log.
    virtual void SetLogFilename(FString NewLogFileName) = 0;
};
