// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Units/UnitBase.h"
#include "TypeOfSpawnable.h"
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
	virtual void BeginPlay() override;

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
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	AEmpire* GetEmpire() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetEmpire(AEmpire* const NewEmpire);
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	ETypeOfSpawnable GetSelectedTypeOfSpawnable() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetSelectedTypeOfSpawnable(const ETypeOfSpawnable NewSelectedTypeOfSpawnable);
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	ETypeOfBuilding GetSelectedTypeOfBuilding() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetSelectedTypeOfBuilding(const ETypeOfBuilding NewSelectedTypeOfBuilding);
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	ETypeOfUnit GetSelectedTypeOfUnit() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetSelectedTypeOfUnit(const ETypeOfUnit NewSelectedTypeOfUnit);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void UpdateBuildingSelection(ETypeOfBuilding NewTypeOfBuilding);
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void UpdateUnitSelection(ETypeOfUnit NewTypeOfUnit);
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void UpdateSpawnableTypeSelection(ETypeOfSpawnable NewTypeOfSpawnable);
	
protected:
	/*Spawnable currently selected to be spawned*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerState")
	TSubclassOf<AActor> ActiveBuilding;

	//todo replace gold etc with empire refs
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState|Empire")
	AEmpire* Empire;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	ETypeOfSpawnable SelectedTypeOfSpawnable = ETypeOfSpawnable::Building;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	ETypeOfBuilding SelectedTypeOfBuilding = ETypeOfBuilding::City;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	ETypeOfUnit SelectedTypeOfUnit = ETypeOfUnit::Soldier;

	// DEPRECATED below
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
