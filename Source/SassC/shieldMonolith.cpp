// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "shieldMonolith.h"

AshieldMonolith::AshieldMonolith() {
	PrimaryActorTick.bCanEverTick = true;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("shieldMonolith Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> shieldMonolithMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_WideCapsule.Shape_WideCapsule'"));
	if (shieldMonolithMesh.Succeeded()) { BuildingMesh->SetStaticMesh(shieldMonolithMesh.Object); }

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("shieldMonolith Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);

}

void AshieldMonolith::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void AshieldMonolith::BeginPlay() {
	Super::BeginPlay();
}

void AshieldMonolith::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AshieldMonolith::ColorBldg_Implementation(FLinearColor PlayerColor, AActor* BuildingRef) {
	BldgMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
	((AshieldMonolith*)BuildingRef)->BuildingMesh->SetMaterial(0, BldgMeshMaterialDynamic);
}

bool AshieldMonolith::ColorBldg_Validate(FLinearColor PlayerColor, AActor* BuildingRef) {
	return true;
}


