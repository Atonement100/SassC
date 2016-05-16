// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "city.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Acity : public AbuildingBase
{
	GENERATED_BODY()
	// Sets default values for this actor's properties
	Acity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	
	
};
