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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		TArray<AActor*> ControlledBuildings;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		TArray<AunitBase*> SelectedUnits;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		TSubclassOf<AActor> ActiveBuilding;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float DeltaGold = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float Gold = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float DeltaIron = 8;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float Iron = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float DeltaFood = 8;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float Food = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float PopulationLimit = 7;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		float CurrentPopulation = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
		FLinearColor PlayerColor;
	
	
};
