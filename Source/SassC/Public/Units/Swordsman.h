// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "Swordsman.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ASwordsman : public AUnitBase
{
	GENERATED_BODY()

	ASwordsman();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
};
