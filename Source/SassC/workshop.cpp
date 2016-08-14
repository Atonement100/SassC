// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "workshop.h"

Aworkshop::Aworkshop() {
	PrimaryActorTick.bCanEverTick = true;

	UpgradeOneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 1 Mesh"));
	UpgradeOneMesh->AttachTo(RootComponent);
}

void Aworkshop::PostInitializeComponents() {
	Super::PostInitializeComponents();
//	BldgMeshMaterialDynamic.Add(BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0)));
	BldgMeshMaterialDynamic.Add(UpgradeOneMesh->CreateDynamicMaterialInstance(0, UpgradeOneMesh->GetMaterial(0)));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Aworkshop::BeginPlay() {
	Super::BeginPlay();
}

void Aworkshop::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (ResetRequired) { ResetPreview(); }
}


void Aworkshop::PreviewUpgrade_Implementation() {
	switch (UpgradeLevel) {
	case 0: { 
		BuildingMesh->SetVisibility(false); 
		UpgradeOneMesh->SetVisibility(true); 
		break; 
	}
	case 1: break;
	default: break;
	}

	ResetRequired = true;
}

void Aworkshop::ResetPreview_Implementation() {
	switch (UpgradeLevel) {
	case 0: { 
		BuildingMesh->SetVisibility(true); 
		UpgradeOneMesh->SetVisibility(false); 
		this->ColorBldg(OwningPlayerColor);
		break; 
	}
	case 1: break;
	default: break;
	}
}

void Aworkshop::NetUpgradeBuilding_Implementation() {
	switch (UpgradeLevel) {
	case 0: { 
		BuildingMesh->SetVisibility(false); 
		UpgradeOneMesh->SetVisibility(true); 
		UpgradeLevel++; 
		this->ColorBldg(OwningPlayerColor);
		break; }
	case 1: break;
	default: break;
	}
}

bool Aworkshop::NetUpgradeBuilding_Validate() {
	return true;
}

void Aworkshop::UpgradeBuilding_Implementation() {
	NetUpgradeBuilding();
}

bool Aworkshop::UpgradeBuilding_Validate() {
	return true;
}

