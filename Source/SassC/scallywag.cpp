// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetSystemLibrary.h"
#include "scallywag.h"

Ascallywag::Ascallywag() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(ScallywagAttackRange / SelectionSphereScaleMod));
}

void Ascallywag::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	AddMeshRelativeLocation(GetMovementComponent()->Velocity.Size());
	if (!(ProcessingMoveToBuildingOrder || ProcessingMoveToUnitOrder || ProcessingMoveToWorldOrder || IsAttacking)) {
		GetMovementComponent()->StopMovementImmediately();
	}
}

void Ascallywag::AddMeshRelativeLocation_Implementation(float Velocity){
	if ((ProcessingMoveToBuildingOrder || ProcessingMoveToUnitOrder || ProcessingMoveToWorldOrder || IsAttacking) && UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), GetActorLocation(), GetActorLocation()-FVector(0,0,80), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4), true, ActorsToIgnore, EDrawDebugTrace::Persistent, Hit, true)) {
		AddActorWorldOffset(FVector(0, 0, .5));
	}
	else if (!UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), GetActorLocation(), GetActorLocation() - FVector(0, 0, 33), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4), true, ActorsToIgnore, EDrawDebugTrace::Persistent, Hit, true)) {//if (GetMesh()->RelativeLocation.Z > -30) {
		AddActorWorldOffset(FVector(0, 0, -.5));
	}
}

bool Ascallywag::AddMeshRelativeLocation_Validate(float Velocity){
	return true;
}

void Ascallywag::NetAddMeshRelativeLocation_Implementation(float Velocity) {

}

bool Ascallywag::NetAddMeshRelativeLocation_Validate(float Velocity) {
	return true;
}