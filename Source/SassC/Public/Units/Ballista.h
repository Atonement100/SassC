// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "Ballista.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ABallista : public AUnitBase
{
	GENERATED_BODY()
	ABallista();

	virtual void Attack_Implementation(AActor* Target) override;
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfEntity;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(30, 25, 5);}

protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Ballista;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackRange123 = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UBoxComponent* UnitCollision;
};
