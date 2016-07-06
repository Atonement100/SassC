// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "workshop.h"

Aworkshop::Aworkshop() {
	PrimaryActorTick.bCanEverTick = true;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Workshop Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> workshopMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Trim_90_In.Shape_Trim_90_In'"));
	if (workshopMesh.Succeeded()) { BuildingMesh->SetStaticMesh(workshopMesh.Object); }

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("workshop Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);

}

void Aworkshop::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Aworkshop::BeginPlay() {
	Super::BeginPlay();
}

void Aworkshop::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


