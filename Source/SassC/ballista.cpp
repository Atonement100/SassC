// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "ballista.h"

Aballista::Aballista() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(BallistaAttackRange / SelectionSphereScaleMod));

	UnitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Unit Collision"));
	UnitCollision->AttachTo(RootComponent);
}

void Aballista::Attack_Implementation(AActor* Target) {
	if (!Target) return;
	Super::Attack_Implementation(Target);
	SpawnProjectile(Target->GetActorLocation());
}
