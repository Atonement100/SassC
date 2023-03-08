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
	virtual ETypeOfEntity GetTypeOfEntity() override {return this->TypeOfEntity;}

protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Peasant;
};
