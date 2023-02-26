// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "BuildingBase.h"
#include "City.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ACity : public ABuildingBase
{
	GENERATED_BODY()

	ACity();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostCreation_Implementation(FLinearColor PlayerColor) override;
	virtual bool PostCreation_Validate(FLinearColor PlayerColor) override;

protected:
	FVector CollisionBounds = FVector(35.0f, 31.0f, 40.0f);
	FVector CollisionDisplacement = FVector(0.0f, 1.0f, 20.0f);
	FVector TraceSize = FVector(35.0f, 31.0f, 10.0f);
	FVector HalfHeight = FVector(0.0f, 0.0f, 20.0f);
};
