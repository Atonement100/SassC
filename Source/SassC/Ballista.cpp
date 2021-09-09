// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Ballista.h"

ABallista::ABallista() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(BallistaAttackRange / SelectionSphereScaleMod));

	UnitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Unit Collision"));
	UnitCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void ABallista::Attack_Implementation(AActor* Target) {
	if (!Target) return;
	Super::Attack_Implementation(Target);
	SpawnProjectile(Target->GetActorLocation());
}
