// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "BuildingBase.h"
#include "Workshop.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AWorkshop : public ABuildingBase
{
	GENERATED_BODY()

	AWorkshop();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual ETypeOfEntity GetTypeOfEntity() override {return this->TypeOfBuilding;}

public:

protected:
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::Workshop;
};
