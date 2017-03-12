// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetStringLibrary.h"
#include "workshop.h"

Aworkshop::Aworkshop() {
	PrimaryActorTick.bCanEverTick = true;

	UpgradeOneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 1 Mesh"));
	UpgradeOneMesh->AttachTo(RootComponent);
}

void Aworkshop::PostInitializeComponents() {
	Super::PostInitializeComponents();
	//This line is to include the second mesh in the dynamic material array, parent only allows for one mesh by default.
	BldgMeshMaterialDynamic.Add(UpgradeOneMesh->CreateDynamicMaterialInstance(0, UpgradeOneMesh->GetMaterial(0)));
}

void Aworkshop::BeginPlay() {
	Super::BeginPlay();
}

void Aworkshop::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (PreviewActive) { PreviewUpgrade(); }
	else if (ResetRequired) { ResetPreview(); }
}

void Aworkshop::PreviewUpgrade_Implementation() {
	switch (UpgradeLevel) {
	case 0: {
		BuildingMesh->SetVisibility(false);
		UpgradeOneMesh->SetVisibility(true);
		this->ColorBldg(FLinearColor::Green);
		break;
	}
	case 1: break;
	default: break;
	}

	ResetRequired = true;
	PreviewActive = false;
}

void Aworkshop::ResetPreview_Implementation() {
	switch (UpgradeLevel) {
	case 0:
		BuildingMesh->SetVisibility(true);
		UpgradeOneMesh->SetVisibility(false);
		this->ColorBldg(OwningPlayerColor);
		break;
	case 1:
		this->ColorBldg(OwningPlayerColor);
		break;
	default: break;
	}
	ResetRequired = false;
}

void Aworkshop::NetUpgradeBuilding_Implementation() {
	switch (UpgradeLevel) {
	case 0:
		BuildingMesh->SetVisibility(false);
		UpgradeOneMesh->SetVisibility(true);
		UpgradeLevel++;
		this->ColorBldg(OwningPlayerColor);
		break;
	case 1:
		this->ColorBldg(OwningPlayerColor);
		break;
	default: break;
	}
	ResetRequired = false;
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


