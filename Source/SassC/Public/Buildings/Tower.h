// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "BuildingBase.h"
#include "Tower.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ATower : public ABuildingBase
{
	GENERATED_BODY()

public:
	ATower();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual ETypeOfEntity GetTypeOfEntity() override {return this->TypeOfBuilding;}

protected:
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::Tower;
};
