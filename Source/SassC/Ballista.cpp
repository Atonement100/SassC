// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Ballista.h"

ABallista::ABallista()
{
	UnitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Unit Collision"));
	UnitCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void ABallista::Attack_Implementation(AActor* Target)
{
	Super::Attack_Implementation(Target);
}

float ABallista::GetAttackRange()
{
	return AttackRange;
}
