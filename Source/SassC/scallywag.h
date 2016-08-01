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
		Ascallywag();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
		float ScallywagAttackRange = 70.0f;
	
	
};
