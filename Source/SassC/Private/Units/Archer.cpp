// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Archer.h"
#include "SassC.h"
#include "Effects/ProjectileSmallArrow.h"

AArcher::AArcher()
{
	
}

void AArcher::BeginPlay()
{
	Super::BeginPlay();
}

void AArcher::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AArcher::SetupPlayerInputComponent(UInputComponent* SetupInputComponent)
{
	Super::SetupPlayerInputComponent(SetupInputComponent);
}

void AArcher::Attack_Implementation(AActor* Target)
{
	Super::Attack_Implementation(Target);
}

bool AArcher::Attack_Validate(AActor* Target)
{
	return true;
}
