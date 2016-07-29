// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "gate.h"

// Fill out your copyright notice in the Description page of Project Settings.

Agate::Agate() {
	PrimaryActorTick.bCanEverTick = true;

	GateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gate Mesh"));
	GateMesh->AttachTo(RootComponent);

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	
}

void Agate::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = GateMesh->CreateDynamicMaterialInstance(0, GateMesh->GetMaterial(0));

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

void Agate::ColorBldg_Implementation(FLinearColor PlayerColor)
{
	BldgMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
	this->GateMesh->SetMaterial(0, BldgMeshMaterialDynamic);
}

bool Agate::ColorBldg_Validate(FLinearColor PlayerColor)
{
	return true;
}





