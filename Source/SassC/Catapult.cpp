// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Catapult.h"

ACatapult::ACatapult()
{
	UnitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Unit Collision"));
	UnitCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

float ACatapult::GetAttackRange()
{
	return AttackRange;
}
