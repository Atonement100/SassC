// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/GameplayStatics.h"
#include "ParticleDefinitions.h"
#include "shieldMonolith.h"

AshieldMonolith::AshieldMonolith() {
	PrimaryActorTick.bCanEverTick = true;
}

void AshieldMonolith::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void AshieldMonolith::BeginPlay() {
	Super::BeginPlay();
}

void AshieldMonolith::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AshieldMonolith::PostCreation_Implementation(FLinearColor PlayerColor) {
	UParticleSystemComponent* BubbleComponent = UGameplayStatics::SpawnEmitterAttached(BubblePSys, BuildingMesh, NAME_None, BubblePSysLocation);
	if(BubbleComponent) AttachedParticleSystems.Add(BubbleComponent);
}

USceneComponent* AshieldMonolith::GetBeamSocket()
{
	if (BeamPSysSocket) return BeamPSysSocket;
	return nullptr;
}

float AshieldMonolith::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health <= 0.0f) {
		for (UParticleSystemComponent* PSys : AttachedParticleSystems) {
			if (PSys) { PSys->DeactivateSystem(); PSys->DestroyComponent(); }
		}
	}

	return DamageAmount;
}

