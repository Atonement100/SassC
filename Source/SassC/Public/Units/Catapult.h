// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "Catapult.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ACatapult : public AUnitBase
{
	GENERATED_BODY()
	ACatapult();
	virtual ETypeOfEntity GetTypeOfEntity() override {return this->TypeOfEntity;}

protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Catapult;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackRange123 = 60.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UBoxComponent* UnitCollision;
};
