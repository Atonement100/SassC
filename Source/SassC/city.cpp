// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "city.h"

Acity::Acity() {
	PrimaryActorTick.bCanEverTick = true;
	//problematic, try moving rootcomponent here from buildingbase
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("City Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CityMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid'"));
	if (CityMesh.Succeeded()) { BuildingMesh->SetStaticMesh(CityMesh.Object); }
	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("City Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);
}

void Acity::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Acity::BeginPlay() {
	Super::BeginPlay();
}

void Acity::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

