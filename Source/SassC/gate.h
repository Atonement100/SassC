// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "gate.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Agate : public AbuildingBase
{
	GENERATED_BODY()
		
	Agate();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorBldg(FLinearColor PlayerColor);
	virtual void ColorBldg_Implementation(FLinearColor PlayerColor) override;
	virtual bool ColorBldg_Validate(FLinearColor PlayerColor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	USkeletalMeshComponent* GateMesh;
	
};
