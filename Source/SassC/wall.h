// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "wall.generated.h"



/**
 * 
 */
UCLASS()


class SASSC_API Awall : public AbuildingBase
{
	GENERATED_BODY()

	Awall();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	TArray<Awall*> FindWallTowersInRange();
	virtual TArray<AActor*> FindWallTowersInRange_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	void AddConnectedWallSegment(AbuildingBase* NewSegment);
	virtual void AddConnectedWallSegment_Implementation(AbuildingBase* NewSegment);
	virtual bool AddConnectedWallSegment_Validate(AbuildingBase* NewSegment);

protected:
	TArray<AbuildingBase*> ConnectedWalls;

};
