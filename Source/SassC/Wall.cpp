// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "WallSegment.h"
#include "Wall.h"

AWall::AWall()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWall::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AWall::BeginPlay()
{
	Super::BeginPlay();
}

void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AWall::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                        AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health <= 0.0f)
	{
		for (ABuildingBase* Wall : ConnectedWalls)
		{
			if (AWallSegment* WallSegmentCast = Cast<AWallSegment>(Wall))
			{
				if (WallSegmentCast->LeftConnection->IsA(AWall::StaticClass()))
				{
					WallSegmentCast->ChangeMesh(true);
					WallSegmentCast->LeftConnection = nullptr;
				}
				else
				{
					WallSegmentCast->ChangeMesh(false);
					WallSegmentCast->RightConnection = nullptr;
				}
			}
		}
	}

	return DamageAmount;
}

AWall* AWall::GetClosestWallTowerInRange(float Range, TArray<AActor*> ActorsToIgnore)
{
	TArray<AWall*> Walls = this->FindWallTowersInRange(Range, ActorsToIgnore);
	if (Walls.Num() == 0) return nullptr;
	else if (Walls.Num() == 1) return Walls[0];
	else
	{
		AWall* WallToReturn = Walls[0];
		float Dist = (Walls[0]->GetActorLocation() - this->GetActorLocation()).SizeSquared();
		for (int WallIndex = 1; WallIndex < Walls.Num(); WallIndex++)
		{
			float CompareDist = (Walls[WallIndex]->GetActorLocation() - this->GetActorLocation()).SizeSquared();
			if (CompareDist < Dist)
			{
				Dist = CompareDist;
				WallToReturn = Walls[WallIndex];
			}
		}
		return WallToReturn;
	}

	return nullptr;
}

TArray<AWall*> AWall::FindWallTowersInRange(float Range, TArray<AActor*> ActorsToIgnore)
{
	TArray<AActor*> IgnoreArray = ActorsToIgnore;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), this->GetActorLocation(),
	                                       this->GetActorLocation() + FVector(0, 0, 1), Range,
	                                       UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3),
	                                       true, IgnoreArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);
	TArray<AWall*> WallsInRange;
	for (FHitResult Hit : SphereHits)
	{
		if (!WallsInRange.Contains(Hit.GetActor()) && Hit.GetActor()->IsA(AWall::StaticClass()))
		{
			float Distance = ((Hit.GetActor()->GetActorLocation()) - this->GetActorLocation()).Size();
			WallsInRange.Add(Cast<AWall>(Hit.GetActor()));
		}
	}

	return WallsInRange;
}

TArray<AActor*> AWall::FindWallTowersInRange_Implementation()
{
	/*
	TArray<AActor*> nullArray;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti_NEW(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 1), 100.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3), true, nullArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);


	GEngine->AddOnScreenDebugMessage(-1, .2f, FColor::Black, UKismetStringLibrary::Conv_IntToString(SphereHits.Num()));
	for (FHitResult Hit : SphereHits) {
		if (!nullArray.Contains(Hit.GetActor()) && Hit.GetActor()->IsA(AWall::StaticClass())) {
			float Distance = UKismetMathLibrary::VSize((Hit.GetActor()->GetActorLocation()) - this->GetActorLocation());
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "Dist: " + UKismetStringLibrary::Conv_FloatToString(Distance));
			nullArray.Add(Hit.GetActor());
		}
	}

	return nullArray;
	*/

	return TArray<AActor*>();
}

void AWall::AddConnectedWallSegment_Implementation(ABuildingBase* NewSegment)
{
	ConnectedWalls.Add(NewSegment);
}

bool AWall::AddConnectedWallSegment_Validate(ABuildingBase* NewSegment)
{
	return true;
}
