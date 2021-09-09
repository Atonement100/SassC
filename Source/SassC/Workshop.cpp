// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetStringLibrary.h"
#include "Workshop.h"

AWorkshop::AWorkshop() {
	PrimaryActorTick.bCanEverTick = true;

	UpgradeOneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 1 Mesh"));
	UpgradeOneMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void AWorkshop::PostInitializeComponents() {
	Super::PostInitializeComponents();
	//This line is to include the second mesh in the dynamic material array, parent only allows for one mesh by default.
	BldgMeshMaterialDynamic.Add(UpgradeOneMesh->CreateDynamicMaterialInstance(0, UpgradeOneMesh->GetMaterial(0)));
}

void AWorkshop::BeginPlay() {
	Super::BeginPlay();
}

void AWorkshop::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (PreviewActive) { PreviewUpgrade(); }
	else if (ResetRequired) { ResetPreview(); }
}

void AWorkshop::PreviewUpgrade_Implementation() {
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

void AWorkshop::ResetPreview_Implementation() {
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

void AWorkshop::NetUpgradeBuilding_Implementation() {
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

bool AWorkshop::NetUpgradeBuilding_Validate() {
	return true;
}

void AWorkshop::UpgradeBuilding_Implementation() {
	NetUpgradeBuilding();
}

bool AWorkshop::UpgradeBuilding_Validate() {
	return true;
}


