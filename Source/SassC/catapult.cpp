// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "catapult.h"

Acatapult::Acatapult() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(CatapultAttackRange / SelectionSphereScaleMod));
}



