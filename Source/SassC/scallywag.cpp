// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "scallywag.h"

Ascallywag::Ascallywag() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(ScallywagAttackRange / SelectionSphereScaleMod));
}

void Ascallywag::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if ((GetMovementComponent()->Velocity.Size() > 0 || IsAttacking) && GetMesh()->RelativeLocation.Z < 60) {
		//GetMesh()->AddForce(FVector(0, 0, 100));
		GetMesh()->AddLocalOffset(FVector(0, 0, .5));
	}
	else if (GetMesh()->RelativeLocation.Z > -30){
		GetMesh()->AddLocalOffset(FVector(0, 0, -.5));
	}
}