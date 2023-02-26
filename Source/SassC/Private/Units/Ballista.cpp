// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Ballista.h"
#include "SassC.h"

ABallista::ABallista()
{
	UnitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Unit Collision"));
	UnitCollision->SetupAttachment(RootComponent);
}

void ABallista::Attack_Implementation(AActor* Target)
{
	Super::Attack_Implementation(Target);
}