// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "ballista.h"

Aballista::Aballista() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(BallistaAttackRange / SelectionSphereScaleMod));
}


