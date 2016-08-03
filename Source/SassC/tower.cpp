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
	UpgradeOneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 2 Mesh"));
	UpgradeOneMesh->AttachTo(RootComponent);
	UpgradeTwoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 3 Mesh"));
	UpgradeTwoMesh->AttachTo(RootComponent);
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
	if (ResetRequired) { ResetPreview(); }
}

void Atower::PreviewUpgrade_Implementation() {
	switch (UpgradeLevel) {
	case 1: { UpgradeOneMesh->SetVisibility(false); UpgradeTwoMesh->SetVisibility(true); break; }
	case 0: { BuildingMesh->SetVisibility(false); UpgradeOneMesh->SetVisibility(true); break; }
	case 2: break;
	default: break;
	}

	ResetRequired = true;
}

void Atower::ResetPreview_Implementation() {
	switch (UpgradeLevel) {
	case 1: { UpgradeOneMesh->SetVisibility(true); UpgradeTwoMesh->SetVisibility(false); break; }
	case 0: { BuildingMesh->SetVisibility(true); UpgradeOneMesh->SetVisibility(false); break; }
	case 2: break;
	default: break;
	}
}

void Atower::NetUpgradeBuilding_Implementation() {
	switch (UpgradeLevel) {
	case 1: { UpgradeOneMesh->SetVisibility(false); UpgradeTwoMesh->SetVisibility(true); UpgradeLevel++; break; }
	case 0: { BuildingMesh->SetVisibility(false); UpgradeOneMesh->SetVisibility(true); UpgradeLevel++; break; }
	case 2: break;
	default: break;
	}
}

bool Atower::NetUpgradeBuilding_Validate() {
	return true;
}

void Atower::UpgradeBuilding_Implementation() {
	NetUpgradeBuilding();
}

bool Atower::UpgradeBuilding_Validate() {
	return true;
}
