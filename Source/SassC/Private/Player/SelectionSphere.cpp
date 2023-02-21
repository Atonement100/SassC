// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "SelectionSphere.h"


// Sets default values
ASelectionSphere::ASelectionSphere()
{
	PrimaryActorTick.bCanEverTick = false;
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
}

// Called when the game starts or when spawned
void ASelectionSphere::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASelectionSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
