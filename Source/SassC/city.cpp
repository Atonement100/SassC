// Fill out your copyright notice in the Description page of Project Settings.


#include "SassC.h"
#include "city.h"

Acity::Acity() {
	PrimaryActorTick.bCanEverTick = true;
	
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("City Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CityMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid'"));
	if (CityMesh.Succeeded()) { BuildingMesh->SetStaticMesh(CityMesh.Object); }
	
	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("City Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);

}

void Acity::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);
	
	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Acity::BeginPlay() {
	Super::BeginPlay();
}

void Acity::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void Acity::ColorBldg_Implementation(FLinearColor PlayerColor, AActor* BuildingRef) {
	BldgMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
	((Acity*)BuildingRef)->BuildingMesh->SetMaterial(0, BldgMeshMaterialDynamic);
}

bool Acity::ColorBldg_Validate(FLinearColor PlayerColor, AActor* BuildingRef) {
	return true;
}
