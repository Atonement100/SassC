// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "projectileSmallArrow.h"
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
	if (!Target) return;
	Super::Attack_Implementation(Target);
	SpawnArrow(Target);
}

bool Aarcher::Attack_Validate(AActor* Target) {
	return true;
}

void Aarcher::SpawnArrow_Implementation(AActor* Target)
{
	//This and related functions are not expressed in unitBase because each projectile-shooting 
	//unit has the potential for very unreleated projectile logic.
	FVector TargetDisplacement = (Target->GetActorLocation() - this->GetActorLocation());

	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	const FVector Location = this->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
	const FRotator Rotation = TargetDisplacement.Rotation();

	AActor* Projectile = GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation, SpawnParams);
	FVector Velocity = Cast<AprojectileSmallArrow>(Projectile)->MovementComponent->Velocity;
	if (Projectile) Projectile->SetLifeSpan((TargetDisplacement).Size() / Velocity.Size());
}

bool Aarcher::SpawnArrow_Validate(AActor* Target)
{
	return true;
}
