// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "wallSegment.h"

AwallSegment::AwallSegment() {
	PrimaryActorTick.bCanEverTick = true;
}

void AwallSegment::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));
}

void AwallSegment::BeginPlay() {
	Super::BeginPlay();
}

void AwallSegment::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}



