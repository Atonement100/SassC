// Fill out your copyright notice in the Description page of Project Settings.

#include "Effects/ProjectileSmallArrow.h"
#include "SassC.h"


AProjectileSmallArrow::AProjectileSmallArrow()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneComponent->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = SceneComponent;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Mesh"));
	ArrowMesh->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	MovementComponent->UpdatedComponent = RootComponent;
}


void AProjectileSmallArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileSmallArrow::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
