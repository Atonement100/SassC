// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "wall.h"

Awall::Awall() {
	PrimaryActorTick.bCanEverTick = true;
	/*
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("wall Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> wallMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Tube.Shape_Tube'"));
	if (wallMesh.Succeeded()) { BuildingMesh->SetStaticMesh(wallMesh.Object); }

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("wall Collision"));
	BuildingCollision->AttachTo(BuildingMesh);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	*/
}

void Awall::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Awall::BeginPlay() {
	Super::BeginPlay();
}

void Awall::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void Awall::FindWallTowersInRange_Implementation()
{
	TArray<AActor*> nullArray;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti_NEW(GetWorld(), this->GetActorLocation(), this->GetActorLocation(), 100.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3), true, nullArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);

	for (FHitResult Hit : SphereHits) {
		if (Hit.GetActor()->IsA(Awall::StaticClass())) {
			FVector Distance = (Hit.GetActor()->GetActorLocation()) - this->GetActorLocation();
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "Dist: " + UKismetStringLibrary::Conv_VectorToString(Distance));
			
		}
	}

}


