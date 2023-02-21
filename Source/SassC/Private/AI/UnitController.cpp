// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "AI/UnitController.h"

void AUnitController::OnPossess()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, "UnitController OnPossess");
}

void AUnitController::BeginPlay()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, "UnitController BeginPlay");
}
