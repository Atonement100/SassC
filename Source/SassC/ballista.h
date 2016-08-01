// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "unitBase.h"
#include "ballista.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Aballista : public AunitBase
{
	GENERATED_BODY()
		Aballista();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
		float BallistaAttackRange = 30.0f;
	
	
};
