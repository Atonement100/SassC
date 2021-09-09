// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "SassilizationHUD.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API USassilizationHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	UWidgetComponent* TestComponent;
	
	
};
