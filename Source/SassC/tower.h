// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "tower.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Atower : public AbuildingBase
{
	GENERATED_BODY()

	Atower();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
#pragma region Upgradeable Building Overrides
	virtual void PreviewUpgrade_Implementation() override;
	virtual void ResetPreview_Implementation() override;
	virtual void NetUpgradeBuilding_Implementation() override;
	virtual bool NetUpgradeBuilding_Validate() override;
	virtual void UpgradeBuilding_Implementation() override;
	virtual bool UpgradeBuilding_Validate() override;
#pragma endregion

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tower")
	UStaticMeshComponent* UpgradeOneMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tower")
	UStaticMeshComponent* UpgradeTwoMesh;

};
