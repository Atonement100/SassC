// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "BuildingBase.h"
#include "Wall.generated.h"


/**
 * 
 */
UCLASS()


class SASSC_API AWall : public ABuildingBase
{
	GENERATED_BODY()

public:
	AWall();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfBuilding;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(1, .75, .2);}

	virtual float GetInfluence() override {return this->Influence;}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, class AActor* DamageCauser) override;
	
	AActor* TempConnection;

	AWall* GetClosestWallTowerInRange(float Range = 150.0f, TArray<AActor*> ActorsToIgnore = TArray<AActor*>());
	TArray<AWall*> FindWallTowersInRange(float Range = 150.0f, TArray<AActor*> ActorsToIgnore = TArray<AActor*>());
	virtual TArray<AActor*> FindWallTowersInRange_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	void AddConnectedWallSegment(ABuildingBase* NewSegment);
	virtual void AddConnectedWallSegment_Implementation(ABuildingBase* NewSegment);
	virtual bool AddConnectedWallSegment_Validate(ABuildingBase* NewSegment);
	TArray<ABuildingBase*> ConnectedWalls;

protected:
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::Wall;
	float Influence = 0.f;
	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( { {ETypeOfEntity::City, FBuildingRequirement::LevelOnly(0)} }) }
	};
};
