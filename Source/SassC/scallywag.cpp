// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "scallywag.h"

Ascallywag::Ascallywag() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(ScallywagAttackRange / SelectionSphereScaleMod));
}



