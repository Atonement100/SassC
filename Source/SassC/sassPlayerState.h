// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "unitBase.h"
#include "GameFramework/PlayerState.h"
#include "sassPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AsassPlayerState : public APlayerState
{
	GENERATED_BODY()
	AsassPlayerState();

public:
	TArray<AActor*> GetControlledBuildings() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	TArray<AActor*> ControlledBuildings;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	TArray<AunitBase*> SelectedUnits;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	TSubclassOf<AActor> ActiveBuilding;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float DeltaGold;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float Gold;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float DeltaIron;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float Iron;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float DeltaFood;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float Food;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float PopulationLimit;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float CurrentPopulation;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		FLinearColor PlayerColor;
	
	
};
