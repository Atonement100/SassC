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
	
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfBuilding;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(100, 100, 25);}
	
	virtual float GetInfluence() override {return this->Influence;}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }

protected:
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::Shrine;
	float Influence = 76.2f;
	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( {
			{ETypeOfEntity::City, FBuildingRequirement::LevelOnly( 0)},
			{ETypeOfEntity::Workshop, FBuildingRequirement::LevelOnly(1)}
		}) }
	};
};
