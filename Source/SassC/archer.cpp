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
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Archer atk");
	if (!Target) return;
	Super::Attack_Implementation(Target);
	SpawnArrow(Target);
}

bool Aarcher::Attack_Validate(AActor* Target) {
	return true;
}

void Aarcher::SpawnArrow_Implementation(AActor* Target)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, "SpawnArrow");
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	const FVector Location = this->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
	const FRotator Rotation = this->GetActorRotation();

	AActor* Projectile = GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation, SpawnParams);
	FVector Velocity = Cast<AprojectileSmallArrow>(Projectile)->MovementComponent->Velocity;
	if (Projectile) Projectile->SetLifeSpan(((Target->GetActorLocation() - this->GetActorLocation()).Size2D() / Velocity.Size()));
}

bool Aarcher::SpawnArrow_Validate(AActor* Target)
{
	return true;
}
