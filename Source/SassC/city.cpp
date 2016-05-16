// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "city.h"

Acity::Acity() {
	PrimaryActorTick.bCanEverTick = true;
	//problematic, try moving rootcomponent here from buildingbase
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> CityMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid'"));
	//if (CityMesh.Object) { BuildingMesh->SetStaticMesh(CityMesh.Object); }
	//BuildingMesh->AttachTo(RootComponent);
}

void Acity::BeginPlay() {
	Super::BeginPlay();
}

void Acity::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

