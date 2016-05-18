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
	
	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void ColorBldg(FLinearColor PlayerColor, AActor* BuildingRef);
	virtual void ColorBldg_Implementation(FLinearColor PlayerColor, AActor* BuildingRef);
	virtual bool ColorBldg_Validate(FLinearColor PlayerColor, AActor* BuildingRef);
};
