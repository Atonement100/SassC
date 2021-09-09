// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "ProjectileSmallArrow.h"


AProjectileSmallArrow::AProjectileSmallArrow()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneComponent->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = SceneComponent;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Mesh"));
	ArrowMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	MovementComponent->UpdatedComponent = RootComponent;
}


void AProjectileSmallArrow::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileSmallArrow::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

