// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "ProjectileSmallArrow.h"
#include "Archer.h"

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

void AArcher::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void AArcher::Attack_Implementation(AActor* Target)
{
	Super::Attack_Implementation(Target);
}

bool AArcher::Attack_Validate(AActor* Target)
{
	return true;
}

float AArcher::GetAttackRange()
{
	return AttackRange;
}
