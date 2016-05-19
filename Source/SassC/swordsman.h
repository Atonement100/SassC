// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "unitBase.h"
#include "swordsman.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Aswordsman : public AunitBase
{
	GENERATED_BODY()

	Aswordsman();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	
};
