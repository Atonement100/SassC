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
	BuildingMesh->SetRelativeLocation(FVector(0, 0, 1));
	
	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("City Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	BuildingCollision->SetRelativeLocation(FVector(0, 0, 50));

	AreaOfInfluence = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Area of Influence"));
	AreaOfInfluence->AttachTo(RootComponent);
	AreaOfInfluence->ComponentTags.Add(USassCStaticLibrary::NoAggroTag());
	AreaOfInfluence->SetWorldScale3D(FVector(0, 0, 0));
	AreaOfInfluence->SetRelativeLocation(FVector(0, 0, 1));

	InfluenceDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Area of Influence Decal"));
	InfluenceDecal->AttachTo(RootComponent);
	InfluenceDecal->SetVisibility(false);
	InfluenceDecal->SetRelativeRotation(FQuat(FRotator(0, -90, 0)));
	InfluenceDecal->SetRelativeLocation(FVector(.5, 3.5, 3.5));
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

void Acity::PostCreation_Implementation(FLinearColor PlayerColor) {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "City has been spawned for a player");
	AreaOfInfluence->SetWorldScale3D(FVector(3.5, 3.5, .5));
	

	UMaterialInstanceDynamic* DynamicDecal = InfluenceDecal->CreateDynamicMaterialInstance();
	DynamicDecal->SetVectorParameterValue("PlayerColor", PlayerColor);
	InfluenceDecal->SetDecalMaterial(DynamicDecal);
	InfluenceDecal->SetVisibility(true);

}

bool Acity::PostCreation_Validate(FLinearColor PlayerColor) {
	return true;
}
