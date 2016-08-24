// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "archer.h"

Aarcher::Aarcher()
{
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(ArcherAttackRange / SelectionSphereScaleMod));
}

void Aarcher::BeginPlay()
{
	Super::BeginPlay();

}

void Aarcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void Aarcher::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void Aarcher::Attack_Implementation(AActor * Target)
{
	//if (!Target) return;
	//Super::Attack(Target);
	/*
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	//const FVector Location = this->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
	const FVector Location = this->GetActorLocation();
	const FRotator Rotation = this->GetActorRotation();

	GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation, SpawnParams);
	*/

}

bool Aarcher::Attack_Validate(AActor* Target) {
	return true;
}
