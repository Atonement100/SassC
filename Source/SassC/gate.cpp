// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "gate.h"

// Fill out your copyright notice in the Description page of Project Settings.

Agate::Agate() {
	PrimaryActorTick.bCanEverTick = true;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> gateMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Trim.Shape_Trim'"));
	if (gateMesh.Succeeded()) { BuildingMesh->SetStaticMesh(gateMesh.Object); }

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);

}

void Agate::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Agate::BeginPlay() {
	Super::BeginPlay();
}

void Agate::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}





