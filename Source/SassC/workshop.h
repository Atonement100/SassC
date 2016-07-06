// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "workshop.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Aworkshop : public AbuildingBase
{
	GENERATED_BODY()

	Aworkshop();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

};
