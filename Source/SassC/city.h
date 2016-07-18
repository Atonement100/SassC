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

	Acity();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostCreation_Implementation(FLinearColor PlayerColor) override;
	virtual bool PostCreation_Validate(FLinearColor PlayerColor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UStaticMeshComponent* AreaOfInfluence;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "City")
	UDecalComponent* InfluenceDecal;

	FVector CollisionBounds = FVector(35.0f, 31.0f, 40.0f);
	FVector CollisionDisplacement = FVector(0.0f, 1.0f, 20.0f);
	FVector TraceSize = FVector(35.0f, 31.0f, 10.0f);
	FVector HalfHeight = FVector(0.0f, 0.0f, 20.0f);
};
