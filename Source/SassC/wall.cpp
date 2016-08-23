// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "wallSegment.h"
#include "wall.h"

Awall::Awall() {
	PrimaryActorTick.bCanEverTick = true;
}

void Awall::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void Awall::BeginPlay() {
	Super::BeginPlay();
}

void Awall::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

Awall* Awall::GetClosestWallTowerInRange(float Range, TArray<AActor*> ActorsToIgnore) {
	TArray<Awall*> Walls = this->FindWallTowersInRange(Range, ActorsToIgnore);
	if (Walls.Num() == 0) return nullptr;
	else if (Walls.Num() == 1) return Walls[0];
	else {
		Awall* WallToReturn = Walls[0];
		float Dist = (Walls[0]->GetActorLocation() - this->GetActorLocation()).SizeSquared();
		for (int WallIndex = 1; WallIndex < Walls.Num(); WallIndex++) {
			float CompareDist = (Walls[WallIndex]->GetActorLocation() - this->GetActorLocation()).SizeSquared();
			if (CompareDist < Dist){
				Dist = CompareDist;
				WallToReturn = Walls[WallIndex];
			}
		}
		return WallToReturn;
	}

	return nullptr;
}

TArray<Awall*> Awall::FindWallTowersInRange(float Range, TArray<AActor*> ActorsToIgnore)
{
	TArray<AActor*> IgnoreArray = ActorsToIgnore;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti_NEW(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 1), Range, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);
	TArray<Awall*> WallsInRange;
	for (FHitResult Hit : SphereHits) {
		if (!WallsInRange.Contains(Hit.GetActor()) && Hit.GetActor()->IsA(Awall::StaticClass())) {
			float Distance = ((Hit.GetActor()->GetActorLocation()) - this->GetActorLocation()).Size();
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


