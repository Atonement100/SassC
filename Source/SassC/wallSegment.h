// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "wallSegment.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AwallSegment : public AbuildingBase
{
	GENERATED_BODY()
	
public:
	AwallSegment();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void Tick(float DeltaSeconds) override;

#pragma region Upgradeable Building Overrides
	virtual void PreviewUpgrade_Implementation() override;
	virtual void ResetPreview_Implementation() override;
	virtual void NetUpgradeBuilding_Implementation() override;
	virtual bool NetUpgradeBuilding_Validate() override;
	virtual void UpgradeBuilding_Implementation() override;
	virtual bool UpgradeBuilding_Validate() override;
#pragma endregion

	TArray<AbuildingBase*> GatePreviewArray;
	UPROPERTY(Replicated)
	AbuildingBase* LeftConnection;
	UPROPERTY(Replicated)
	AbuildingBase* RightConnection;
	/*
	UFUNCTION(Reliable, Server, WithValidation)
	void FixSpawnLocation(FVector RealLocation);
	virtual void FixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool FixSpawnLocation_Validate(FVector RealLocation);
	*/
protected:
	/*
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetFixSpawnLocation(FVector RealLocation);
	virtual void NetFixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool NetFixSpawnLocation_Validate(FVector RealLocation);
	*/
};
