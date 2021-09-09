// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "SelectionSphere.h"


// Sets default values
ASelectionSphere::ASelectionSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
}

// Called when the game starts or when spawned
void ASelectionSphere::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASelectionSphere::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

