// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "projectileSmallArrow.h"


AprojectileSmallArrow::AprojectileSmallArrow()
{
	PrimaryActorTick.bCanEverTick = true;
	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Mesh"));
	ArrowMesh->AttachTo(RootComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	MovementComponent->UpdatedComponent = RootComponent;
}

void AprojectileSmallArrow::BeginPlay()
{
	Super::BeginPlay();
	
}

void AprojectileSmallArrow::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

