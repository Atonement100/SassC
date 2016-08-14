// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "tower.h"

Atower::Atower() {
	PrimaryActorTick.bCanEverTick = true;

	UpgradeOneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 2 Mesh"));
	UpgradeOneMesh->AttachTo(RootComponent);
	UpgradeTwoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 3 Mesh"));
	UpgradeTwoMesh->AttachTo(RootComponent);
}

void Atower::PostInitializeComponents() {
	Super::PostInitializeComponents();
	//BldgMeshMaterialDynamic.Add(BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0)));
	BldgMeshMaterialDynamic.Add(BuildingMesh->CreateDynamicMaterialInstance(1, BuildingMesh->GetMaterial(1)));
	for (int NumMaterials = 0; NumMaterials < UpgradeOneMesh->GetNumMaterials(); NumMaterials++) {
		BldgMeshMaterialDynamic.Add(UpgradeOneMesh->CreateDynamicMaterialInstance(NumMaterials, UpgradeOneMesh->GetMaterial(NumMaterials)));
	}
	for (int NumMaterials = 0; NumMaterials < UpgradeOneMesh->GetNumMaterials(); NumMaterials++) {
		BldgMeshMaterialDynamic.Add(UpgradeTwoMesh->CreateDynamicMaterialInstance(NumMaterials, UpgradeTwoMesh->GetMaterial(NumMaterials)));
	}
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
	case 1: { 
		UpgradeOneMesh->SetVisibility(false); 
		UpgradeTwoMesh->SetVisibility(true); 
		break; 
	}
	case 0: { 
		BuildingMesh->SetVisibility(false); 
		UpgradeOneMesh->SetVisibility(true); 
		break; 
	}
	case 2: break;
	default: break;
	}

	ResetRequired = true;
}

void Atower::ResetPreview_Implementation() {
	switch (UpgradeLevel) {
	case 1: { 
		UpgradeOneMesh->SetVisibility(true); 
		UpgradeTwoMesh->SetVisibility(false); 
		this->ColorBldg(OwningPlayerColor, 1); 
		break; 
	}
	case 0: { 
		BuildingMesh->SetVisibility(true); 
		UpgradeOneMesh->SetVisibility(false); 
		this->ColorBldg(OwningPlayerColor, 0);
		break; }
	case 2: break;
	default: break;
	}
}

void Atower::NetUpgradeBuilding_Implementation() {
	switch (UpgradeLevel) {
	case 1: { 
		UpgradeOneMesh->SetVisibility(false); 
		UpgradeTwoMesh->SetVisibility(true); 
		UpgradeLevel++; 
		this->ColorBldg(OwningPlayerColor);
		break; 
	}
	case 0: { 
		BuildingMesh->SetVisibility(false); 
		UpgradeOneMesh->SetVisibility(true); 
		UpgradeLevel++; 
		this->ColorBldg(OwningPlayerColor);
		break; 
	}
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
