// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/PlayerController.h"
#include "Gamemode/Sassilization/SassGameManager.h"
#include "SassPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ASassPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASassPlayerController();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool IsSpawnableRequestValid();
	UFUNCTION(BlueprintCallable)
	bool RequestSpawnable();
	UFUNCTION(BlueprintCallable)
	bool RequestBuildingSpawn();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Gamemode")
	ASassGameManager* SassGameManager; 
};
