// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "Archer.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AArcher : public AUnitBase
{
	GENERATED_BODY()

	AArcher();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* SetupInputComponent) override;

	virtual void Attack_Implementation(AActor* Target) override;
	virtual bool Attack_Validate(AActor* Target) override;
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfEntity;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(11, 9, 0, 1);}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }
	
protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Archer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackRange123 = 50.0f;
	
	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( {
			{ETypeOfEntity::City, FBuildingRequirement::LevelOnly(0)},
			{ETypeOfEntity::Workshop, FBuildingRequirement::LevelOnly(0)}
		}) }
	};
};
