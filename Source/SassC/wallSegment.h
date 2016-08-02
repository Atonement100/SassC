// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "wallSegment.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AwallSegment : public AbuildingBase
{
	GENERATED_BODY()
	
	AwallSegment();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:

	
	
};
