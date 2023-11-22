// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"


void UPlayerWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UPlayerWidget::SetDescription(FText text) {
	if (TextBox) {
		TextBox->SetText(text);
		FTimerHandle time;
		GetWorld()->GetTimerManager().SetTimer(time, [&]()
			{
				TextBox->SetText(FText::GetEmpty());
				GetWorld()->GetTimerManager().ClearTimer(time);
			}, 0.3f, false);
	}
}