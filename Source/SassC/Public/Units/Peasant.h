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
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(0, 0, 0);}

protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Peasant;
};
