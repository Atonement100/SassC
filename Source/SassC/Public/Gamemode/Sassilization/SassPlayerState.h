// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Units/UnitBase.h"
#include "TypeOfEntity.h"
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
	ETypeOfEntity GetSelectedTypeOfEntity() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetSelectedTypeOfEntity(const ETypeOfEntity NewSelectedTypeOfSpawnable);
	bool IsAllowedToSpawnBuilding() const;
	void SetAllowedToSpawnBuilding(const bool bAllowedToSpawnBuilding);

protected:
	/*Spawnable currently selected to be spawned*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerState")
	TSubclassOf<AActor> ActiveBuilding;

	//todo replace gold etc with empire refs
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState|Empire")
	AEmpire* Empire;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState|Empire")
	bool bAllowedToSpawnBuilding = true;;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	ETypeOfEntity SelectedTypeOfEntity = ETypeOfEntity::City;
};
