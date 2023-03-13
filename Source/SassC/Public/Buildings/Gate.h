// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuildingBase.h"
#include "Gate.generated.h"

/**
 * 
 */

UCLASS()
class SASSC_API AGate : public ABuildingBase
{
	GENERATED_BODY()

public:
	AGate();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfBuilding;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts(8, 1, 5);}

	virtual float GetInfluence() override {return this->Influence;}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }

	UFUNCTION()
	virtual void OnOverlapBegin_GateTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                        const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_GateTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gate")
	bool ShouldBeOpen;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gate")
	UBoxComponent* OpenTrigger;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gate")
	int32 UnitsInRange;
	
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::Gate;
	float Influence = 0.f;
	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( {
			{ETypeOfEntity::City, FBuildingRequirement::LevelOnly(0)},
			{ETypeOfEntity::Workshop, FBuildingRequirement::LevelOnly(0)}
		}) }
	};
};
