// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "unitBase.h"
#include "catapult.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Acatapult : public AunitBase
{
	GENERATED_BODY()
		Acatapult();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
		float CatapultAttackRange = 60.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
		UBoxComponent* UnitCollision;
	
};
