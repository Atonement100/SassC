// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "shrine.h"

Ashrine::Ashrine() {
	PrimaryActorTick.bCanEverTick = true;
	/*
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("shrine Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> shrineMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Pipe_180.Shape_Pipe_180'"));
	if (shrineMesh.Succeeded()) { BuildingMesh->SetStaticMesh(shrineMesh.Object); }

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("shrine Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	*/
}

void Ashrine::PostInitializeComponents() {
	Super::PostInitializeComponents();
//	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Ashrine::BeginPlay() {
	Super::BeginPlay();
}

void Ashrine::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


