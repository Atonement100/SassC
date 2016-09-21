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
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Archer atk");
	if (!Target) return;
	Super::Attack_Implementation(Target);
	
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	const FVector Location = this->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
	const FRotator Rotation = this->GetActorRotation();

	GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation, SpawnParams);
}

bool Aarcher::Attack_Validate(AActor* Target) {
	return true;
}
