// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/UnitController.h"
#include "SassC.h"

void AUnitController::OnPossess(APawn* InPawn)
{
	UE_LOG(Sassilization, Display, TEXT("UnitController OnPossess"));
}

void AUnitController::BeginPlay()
{
	UE_LOG(Sassilization, Display, TEXT("UnitController BeginPlay"));
}
