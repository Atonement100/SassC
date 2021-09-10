// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Tower.h"

ATower::ATower()
{
	PrimaryActorTick.bCanEverTick = true;

	UpgradeOneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 2 Mesh"));
	UpgradeOneMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	UpgradeTwoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Level 3 Mesh"));
	UpgradeTwoMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void ATower::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//These are to include the additional meshes and materials in the dynamic material array, parent only adds one mesh and material by default.
	BldgMeshMaterialDynamic.Add(BuildingMesh->CreateDynamicMaterialInstance(1, BuildingMesh->GetMaterial(1)));
	for (int NumMaterials = 0; NumMaterials < UpgradeOneMesh->GetNumMaterials(); NumMaterials++)
	{
		BldgMeshMaterialDynamic.Add(
			UpgradeOneMesh->CreateDynamicMaterialInstance(NumMaterials, UpgradeOneMesh->GetMaterial(NumMaterials)));
	}
	for (int NumMaterials = 0; NumMaterials < UpgradeOneMesh->GetNumMaterials(); NumMaterials++)
	{
		BldgMeshMaterialDynamic.Add(
			UpgradeTwoMesh->CreateDynamicMaterialInstance(NumMaterials, UpgradeTwoMesh->GetMaterial(NumMaterials)));
	}
}

void ATower::BeginPlay()
{
	Super::BeginPlay();
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PreviewActive) { PreviewUpgrade(); }
	else if (ResetRequired) { ResetPreview(); }
}

void ATower::PreviewUpgrade_Implementation()
{
	switch (UpgradeLevel)
	{
	case 1:
		{
			UpgradeOneMesh->SetVisibility(false);
			UpgradeTwoMesh->SetVisibility(true);
			this->ColorBldg(FLinearColor::Green);
			break;
		}
	case 0:
		{
			BuildingMesh->SetVisibility(false);
			UpgradeOneMesh->SetVisibility(true);
			this->ColorBldg(FLinearColor::Green);
			break;
		}
	case 2: break;
	default: break;
	}

	ResetRequired = true;
	PreviewActive = false;
}

void ATower::ResetPreview_Implementation()
{
	switch (UpgradeLevel)
	{
	case 1:
		{
			UpgradeOneMesh->SetVisibility(true);
			UpgradeTwoMesh->SetVisibility(false);
			this->ColorBldg(OwningPlayerColor);
			break;
		}
	case 0:
		{
			BuildingMesh->SetVisibility(true);
			UpgradeOneMesh->SetVisibility(false);
			this->ColorBldg(OwningPlayerColor);
			break;
		}
	case 2: break;
	default: break;
	}
}

void ATower::NetUpgradeBuilding_Implementation()
{
	switch (UpgradeLevel)
	{
	case 1:
		{
			UpgradeOneMesh->SetVisibility(false);
			UpgradeTwoMesh->SetVisibility(true);
			UpgradeLevel++;
			this->ColorBldg(OwningPlayerColor);
			break;
		}
	case 0:
		{
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

bool ATower::NetUpgradeBuilding_Validate()
{
	return true;
}

void ATower::UpgradeBuilding_Implementation()
{
	NetUpgradeBuilding();
}

bool ATower::UpgradeBuilding_Validate()
{
	return true;
}
