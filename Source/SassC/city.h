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
	virtual void PostCreation_Implementation() override;
	virtual bool PostCreation_Validate() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UStaticMeshComponent* AreaOfInfluence;

	const FName NoAggroTag = "NULLIFY_OVERLAP"; //this should never ever be changed unless also changed in sassplayer
};
