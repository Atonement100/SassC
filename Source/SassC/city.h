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

};
