// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "BuildingBase.h"
#include "Engine/StaticMesh.h"
#include "City.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ACity : public ABuildingBase
{
	GENERATED_BODY()

	ACity();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostCreation_Implementation(FLinearColor PlayerColor) override;
	virtual bool PostCreation_Validate(FLinearColor PlayerColor) override;
	void TimedExpansion();
	virtual ETypeOfEntity GetTypeOfEntity() const override { return this->TypeOfBuilding; }
	virtual FResourceCosts GetResourceCosts() const override { return FResourceCosts(50, 50, 32); }
	// virtual FBox GetSpawnBoundingBox(int ForLevel) const override 
	// {
	// 	return Cast<UStaticMesh>(this->AvailableBuildingMeshes[0])->GetBoundingBox();
	// }
	virtual void WhenBuilt() override;
	
	virtual float GetInfluence() override {return this->Influence;}
	virtual void UpdateInfluence() override {this->Influence = FMath::Max(0, this->Influence - 32.1469f);};
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }

protected:
	FVector CollisionBounds = FVector(35.0f, 31.0f, 40.0f);
	FVector CollisionDisplacement = FVector(0.0f, 1.0f, 20.0f);
	FVector TraceSize = FVector(35.0f, 31.0f, 10.0f);
	FVector HalfHeight = FVector(0.0f, 0.0f, 20.0f);
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::City;
	float Influence = 95.25f;
	TArray<FBuildingRequirements> LevelRequirements = {};
};
