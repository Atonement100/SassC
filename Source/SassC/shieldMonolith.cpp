// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ParticleDefinitions.h"
#include "shieldMonolith.h"

AshieldMonolith::AshieldMonolith() {
	PrimaryActorTick.bCanEverTick = true;
	BeamPSysSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Beam Socket"));
	BeamPSysSocket->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
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

void AshieldMonolith::SpawnBeamEmitter_Implementation(UParticleSystem* PSysToSpawn, AshieldMonolith* SourceMonolith, AshieldMonolith* TargetMonolith)
{
	FVector Target = TargetMonolith->GetBeamSocket()->GetComponentLocation();
	FVector Source = SourceMonolith->GetBeamSocket()->GetComponentLocation();

	UParticleSystemComponent* NewPSys = UGameplayStatics::SpawnEmitterAttached(PSysToSpawn, this->GetBeamSocket());
	if (NewPSys) {
		NewPSys->SetBeamTargetPoint(0, Target, 0);
		NewPSys->SetBeamSourcePoint(0, Source, 0);
		SourceMonolith->AttachedParticleSystems.Add(NewPSys);
		TargetMonolith->AttachedParticleSystems.Add(NewPSys);
	}
}

bool AshieldMonolith::SpawnBeamEmitter_Validate(UParticleSystem* PSysToSpawn, AshieldMonolith* SourceMonolith, AshieldMonolith* TargetMonolith)
{
	return true;
}

void AshieldMonolith::DestroyConnectedBeamEmitters_Implementation()
{
	for (UParticleSystemComponent* PSys : AttachedParticleSystems) {
		if (PSys) { PSys->DeactivateSystem(); PSys->DestroyComponent(); }
	}
}

bool AshieldMonolith::DestroyConnectedBeamEmitters_Validate()
{
	return true;
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
		DestroyConnectedBeamEmitters();
	}

	return DamageAmount;
}

TArray<AshieldMonolith*> AshieldMonolith::FindMonolithsInRange(float Range, TArray<AActor*> ActorsToIgnore)
{
	TArray<AActor*> IgnoreArray = ActorsToIgnore;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 1), Range, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);
	TArray<AshieldMonolith*> WallsInRange;
	for (FHitResult Hit : SphereHits) {
		if (!WallsInRange.Contains(Hit.GetActor()) && Hit.GetActor()->IsA(AshieldMonolith::StaticClass()) && Hit.GetActor() != this) {
			float Distance = ((Hit.GetActor()->GetActorLocation()) - this->GetActorLocation()).Size();
			WallsInRange.Add(Cast<AshieldMonolith>(Hit.GetActor()));
		}
	}

	return WallsInRange;
}