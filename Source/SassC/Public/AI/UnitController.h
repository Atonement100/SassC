// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "UnitController.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AUnitController : public AAIController
{
	GENERATED_BODY()

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
};
