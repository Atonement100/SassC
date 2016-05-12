// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "sassPauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API UsassPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pause Menu")
		FString MyNewWidgetName;
		
	
};
