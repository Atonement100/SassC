// Fill out your copyright notice in the Description page of Project Settings.


#include "SassC.h"
#include "SassCStaticLibrary.h"
#include "Kismet/KismetStringLibrary.h"
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

	AreaOfInfluence = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Area of Influence"));
	AreaOfInfluence->AttachTo(RootComponent);
	AreaOfInfluence->ComponentTags.Add(USassCStaticLibrary::NoAggroTag());
	AreaOfInfluence->SetWorldScale3D(FVector(0, 0, 0));
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

void Acity::PostCreation_Implementation() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "City has been spawned for a player");
	AreaOfInfluence->SetRelativeScale3D(FVector(2.5, 2.5, .5));
	
}

bool Acity::PostCreation_Validate() {
	return true;
}
