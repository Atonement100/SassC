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

protected:
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
