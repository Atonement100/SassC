// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "shrine.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Ashrine : public AbuildingBase
{
	GENERATED_BODY()

		Ashrine();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

};