// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "unitBase.h"
#include "scallywag.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Ascallywag : public AunitBase
{
	GENERATED_BODY()

public:
	Ascallywag();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	virtual void UpdateMaterial(FLinearColor PlayerColor) override;

	virtual void Attack_Implementation(AActor* Target) override;
	virtual bool Attack_Validate(AActor* Target) override;
protected:
	UFUNCTION(Unreliable, NetMulticast, WithValidation)
	void SpawnProjectile(FVector TargetLocation);
	virtual void SpawnProjectile_Implementation(FVector TargetLocation);
	virtual bool SpawnProjectile_Validate(FVector TargetLocation);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float ScallywagAttackRange = 70.0f;
	
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
};
