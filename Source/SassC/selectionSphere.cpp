// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "selectionSphere.h"


// Sets default values
AselectionSphere::AselectionSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
}

// Called when the game starts or when spawned
void AselectionSphere::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AselectionSphere::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

