// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "wallSegment.generated.h"

class Agate;
class Awall;

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
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void Tick(float DeltaSeconds) override;

#pragma region Upgradeable Building Overrides
	void TryRemove(AbuildingBase * RemoveFrom, bool IsLeftConnection);
	int TryConnection(AbuildingBase * Connection, TArray<AbuildingBase*> &ConnectedBldgs, int8 Depth, bool TryLeft);
	void HideMesh() override;
	void ShowMesh() override;
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

	UFUNCTION(Reliable, Server, WithValidation)
	void ChangeMesh(bool LeftConnectionDestroyed = false);
	virtual void ChangeMesh_Implementation(bool LeftConnectionDestroyed = false);
	virtual bool ChangeMesh_Validate(bool LeftConnectionDestroyed = false);
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetChangeMesh(bool LeftConnectionDestroyed = false);
	virtual void NetChangeMesh_Implementation(bool LeftConnectionDestroyed = false);
	virtual bool NetChangeMesh_Validate(bool LeftConnectionDestroyed = false);

protected:
	AActor* TempGate;
	AActor* TempLeftWall;
	AActor* TempRightWall;
	UPROPERTY(Replicated)
	int8 DamageLevel = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wall Segment")
	UStaticMeshComponent* DamageOneMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wall Segment")
	UStaticMeshComponent* DamageTwoMesh;
	
	/*
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetFixSpawnLocation(FVector RealLocation);
	virtual void NetFixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool NetFixSpawnLocation_Validate(FVector RealLocation);
	*/
};
