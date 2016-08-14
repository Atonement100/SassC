// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "wallSegment.h"
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
//	BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));

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

TArray<Awall*> Awall::FindWallTowersInRange()
{
	TArray<AActor*> nullArray;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti_NEW(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 1), 150.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3), true, nullArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);
	TArray<Awall*> WallsInRange;
	for (FHitResult Hit : SphereHits) {
		if (!WallsInRange.Contains(Hit.GetActor()) && Hit.GetActor()->IsA(Awall::StaticClass())) {
			float Distance = UKismetMathLibrary::VSize((Hit.GetActor()->GetActorLocation()) - this->GetActorLocation());
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "Dist: " + UKismetStringLibrary::Conv_FloatToString(Distance));
			WallsInRange.Add(Cast<Awall>(Hit.GetActor()));
		}
	}

	return WallsInRange;
}

TArray<AActor*> Awall::FindWallTowersInRange_Implementation()
{
	/*
	TArray<AActor*> nullArray;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti_NEW(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 1), 100.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3), true, nullArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);


	GEngine->AddOnScreenDebugMessage(-1, .2f, FColor::Black, UKismetStringLibrary::Conv_IntToString(SphereHits.Num()));
	for (FHitResult Hit : SphereHits) {
		if (!nullArray.Contains(Hit.GetActor()) && Hit.GetActor()->IsA(Awall::StaticClass())) {
			float Distance = UKismetMathLibrary::VSize((Hit.GetActor()->GetActorLocation()) - this->GetActorLocation());
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "Dist: " + UKismetStringLibrary::Conv_FloatToString(Distance));
			nullArray.Add(Hit.GetActor());
		}
	}

	return nullArray;
	*/

	return TArray<AActor*>();
}

void Awall::AddConnectedWallSegment_Implementation(AbuildingBase * NewSegment)
{
	ConnectedWalls.Add(NewSegment);
}

bool Awall::AddConnectedWallSegment_Validate(AbuildingBase * NewSegment)
{
	return true;
}


