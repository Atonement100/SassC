// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuildingBase.h"
#include "Shrine.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AShrine : public ABuildingBase
{
	GENERATED_BODY()

	AShrine();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
