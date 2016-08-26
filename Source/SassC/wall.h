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

public:
	Awall();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	AActor* TempConnection;

	Awall * GetClosestWallTowerInRange(float Range = 150.0f, TArray<AActor*> ActorsToIgnore = TArray<AActor*>());
	TArray<Awall*> FindWallTowersInRange(float Range = 150.0f, TArray<AActor*> ActorsToIgnore = TArray<AActor*>());
	virtual TArray<AActor*> FindWallTowersInRange_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	void AddConnectedWallSegment(AbuildingBase* NewSegment);
	virtual void AddConnectedWallSegment_Implementation(AbuildingBase* NewSegment);
	virtual bool AddConnectedWallSegment_Validate(AbuildingBase* NewSegment);
	TArray<AbuildingBase*> ConnectedWalls;

protected:
	

};
