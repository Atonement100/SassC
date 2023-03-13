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
	
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfBuilding;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(22, 16, 3);}

	virtual float GetInfluence() override {return this->Influence;}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }
	
protected:
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::Tower;
	float Influence = 0.f;
	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( {
			{ETypeOfEntity::City, FBuildingRequirement::AmountOnly(1)},
		}) },
		{ FBuildingRequirements( {
			{ETypeOfEntity::Workshop, FBuildingRequirement(1, 0)},
		}) },
		{ FBuildingRequirements( {
			{ETypeOfEntity::Workshop, FBuildingRequirement(2, 1)},
		}) },
	};
};
