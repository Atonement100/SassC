// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "ProjectileSmallArrow.h"
#include "Archer.h"

AArcher::AArcher()
{
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(ArcherAttackRange / SelectionSphereScaleMod));
}

void AArcher::BeginPlay()
{
	Super::BeginPlay();

}

void AArcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArcher::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AArcher::Attack_Implementation(AActor * Target)
{
	if (!Target) return;
	Super::Attack_Implementation(Target);
	SpawnProjectile(Target->GetActorLocation());
}

bool AArcher::Attack_Validate(AActor* Target) {
	return true;
}
