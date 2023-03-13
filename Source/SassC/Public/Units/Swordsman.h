// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "UnitBase.h"
#include "Swordsman.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ASwordsman : public AUnitBase
{
	GENERATED_BODY()

	ASwordsman();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* SetupInputComponent) override;
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfEntity;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(10, 12, .5);}

protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Swordsman;
};
