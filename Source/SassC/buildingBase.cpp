// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "buildingBase.h"

AbuildingBase::AbuildingBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneComponent->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = SceneComponent;
}

void AbuildingBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AbuildingBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AbuildingBase::UpdateMaterial(FLinearColor PlayerColor, AActor* BuildingRef) {
	ColorBldg(PlayerColor, BuildingRef);
}

void AbuildingBase::ColorBldg_Implementation(FLinearColor PlayerColor, AActor* BuildingRef) {
	BldgMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
	((AbuildingBase*)BuildingRef)->BuildingMesh->SetMaterial(0, BldgMeshMaterialDynamic);
}

bool AbuildingBase::ColorBldg_Validate(FLinearColor PlayerColor, AActor* BuildingRef) {
	return true;
}

