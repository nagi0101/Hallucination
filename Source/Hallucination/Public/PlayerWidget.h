// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class HALLUCINATION_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextBox;

	void SetDescription(FText text);
	
};
