// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "GameFramework/PlayerState.h"
#include "SassPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ASassPlayerState : public APlayerState
{
	GENERATED_BODY()
	ASassPlayerState();

public:
	/*All controlled buildings*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	TArray<AActor*> ControlledBuildings;
	/*Currently selected units*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	TArray<AUnitBase*> SelectedUnits;
	/*Playercolor that is set on server join, set in postlogin gamemode*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	FLinearColor PlayerColor;

protected:
	/*Spawnable currently selected to be spawned*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerState")
	TSubclassOf<AActor> ActiveBuilding;
	/*Current rate of gold gain*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float DeltaGold = 1;
	/*Current quantity of gold*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float Gold = 50;
	/*Current rate of iron gain*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float DeltaIron = 8;
	/*Current quantity of iron*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float Iron = 50;
	/*Current rate of food gain*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float DeltaFood = 8;
	/*Current quantity of food*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float Food = 50;
	/*Maximum population based on currently owned buildings*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float PopulationLimit = 7;
	/*Current population (number of units spawned)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float CurrentPopulation = 0;
};
