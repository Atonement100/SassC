// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/City.h"
#include "SassC.h"

ACity::ACity()
{
}

void ACity::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACity::BeginPlay()
{
	Super::BeginPlay();
}

void ACity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACity::PostCreation_Implementation(FLinearColor PlayerColor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "City has been spawned for a player");
}

bool ACity::PostCreation_Validate(FLinearColor PlayerColor)
{
	return true;
}
