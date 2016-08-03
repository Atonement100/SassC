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
	UFUNCTION(Unreliable, Client)
	void PreviewUpgrade();
	virtual void PreviewUpgrade_Implementation();

	UFUNCTION(Unreliable, Client)
	void ResetPreview();
	virtual void ResetPreview_Implementation();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tower")
	UStaticMeshComponent* UpgradeOneMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tower")
	UStaticMeshComponent* UpgradeTwoMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tower")
	uint8 UpgradeLevel = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tower")
	bool ResetRequired;

};
