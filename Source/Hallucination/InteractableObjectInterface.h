// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
//#include "UObject/Interface.h"
#include "InteractableObjectInterface.generated.h"


UINTERFACE(Blueprintable)
class UInteractableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/// <summary>
/// Interface for interatable objects (usually called by 'E' key)
/// </summary>
class HALLUCINATION_API IInteractableObjectInterface
{
	GENERATED_BODY()

public:
	/// <summary>
	/// Interact with object
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InteractThis();
};
