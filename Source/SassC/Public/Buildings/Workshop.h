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

	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfBuilding;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(80, 90, 25);}

	virtual float GetInfluence() override {return this->Influence;}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }

protected:
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::Workshop;
	float Influence = 76.2f;
	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( {
			{ETypeOfEntity::City, FBuildingRequirement::AmountOnly(1)}
		}) },
		{ FBuildingRequirements( {
			{ETypeOfEntity::City, FBuildingRequirement::AmountOnly(1)}
		}) }
	};
};
