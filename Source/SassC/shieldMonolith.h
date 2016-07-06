// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "shieldMonolith.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AshieldMonolith : public AbuildingBase
{
	GENERATED_BODY()
	
	AshieldMonolith();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	

	
};
