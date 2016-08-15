// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetStringLibrary.h"
#include "wallSegment.h"

AwallSegment::AwallSegment() {
	PrimaryActorTick.bCanEverTick = true;

}

void AwallSegment::PostInitializeComponents() {
	Super::PostInitializeComponents();
	//BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));
}

void AwallSegment::BeginPlay() {
	Super::BeginPlay();
}

void AwallSegment::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

/*
void AwallSegment::FixSpawnLocation_Implementation(FVector RealLocation)
{
	NetFixSpawnLocation(RealLocation);
}

bool AwallSegment::FixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

void AwallSegment::NetFixSpawnLocation_Implementation(FVector RealLocation)
{
	Role = ROLE_Authority;
	SetActorLocation(RealLocation + FVector(10));
	SetActorLocation(RealLocation);
}

bool AwallSegment::NetFixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}
*/