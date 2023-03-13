// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "Scallywag.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AScallywag : public AUnitBase
{
	GENERATED_BODY()

public:
	AScallywag();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	virtual void UpdateMaterial(FLinearColor PlayerColor) override;

	virtual void Attack_Implementation(AActor* Target) override;
	virtual bool Attack_Validate(AActor* Target) override;
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfEntity;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(17, 12, 1, 1.5);}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }

protected:
	ETypeOfEntity TypeOfEntity = ETypeOfEntity::Scallywag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackRange123 = 70.0f;

	UFUNCTION(Reliable, Server, WithValidation)
	void AddMeshRelativeLocation(float Velocity);
	virtual void AddMeshRelativeLocation_Implementation(float Velocity);
	virtual bool AddMeshRelativeLocation_Validate(float Velocity);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetAddMeshRelativeLocation(float Velocity);
	virtual void NetAddMeshRelativeLocation_Implementation(float Velocity);
	virtual bool NetAddMeshRelativeLocation_Validate(float Velocity);

	const TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	FHitResult Hit;

	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( {
			{ETypeOfEntity::City, FBuildingRequirement::LevelOnly(0)},
			{ETypeOfEntity::Workshop, FBuildingRequirement::LevelOnly(0)}
		}) }
	};
};
