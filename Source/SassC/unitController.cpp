// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "unitController.h"

void AunitController::OnPossess() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, "possessedf!!!!");
}

void AunitController::BeginPlay() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, "pdddddddddddossessedf!!!!");
}
