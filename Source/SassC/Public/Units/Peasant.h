// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "Peasant.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API APeasant : public AUnitBase
{
	GENERATED_BODY()
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfEntity;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(0, 0, 0, 0);}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }
	virtual bool CanPlayerSpawn() const override {return false;}

protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Peasant;
	TArray<FBuildingRequirements> LevelRequirements = {};
};
