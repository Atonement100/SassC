// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "SassPauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API USassPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pause Menu")
		FString MyNewWidgetName;
		
	
};
