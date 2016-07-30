// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "tower.h"

Atower::Atower() {
	PrimaryActorTick.bCanEverTick = true;
	/*
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tower Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> towerMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_TriPyramid.Shape_TriPyramid'"));
	if (towerMesh.Succeeded()) { BuildingMesh->SetStaticMesh(towerMesh.Object); }

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("tower Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	*/
}

void Atower::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Atower::BeginPlay() {
	Super::BeginPlay();
}

void Atower::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


