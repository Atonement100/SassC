// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Catapult.h"
#include "SassC.h"

ACatapult::ACatapult()
{
	UnitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Unit Collision"));
	UnitCollision->SetupAttachment(RootComponent);
}
