// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "unitController.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AunitController : public AAIController
{
	GENERATED_BODY()

	virtual void OnPossess();
	virtual void BeginPlay() override;

};
