// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "tower.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Atower : public AbuildingBase
{
	GENERATED_BODY()

	Atower();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
};
