// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ParticleDefinitions.h"
#include "ShieldMonolith.h"

AShieldMonolith::AShieldMonolith() {
	PrimaryActorTick.bCanEverTick = true;
	BeamPSysSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Beam Socket"));
	BeamPSysSocket->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void AShieldMonolith::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void AShieldMonolith::BeginPlay() {
	Super::BeginPlay();
}

void AShieldMonolith::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AShieldMonolith::PostCreation_Implementation(FLinearColor PlayerColor) {
	UParticleSystemComponent* BubbleComponent = UGameplayStatics::SpawnEmitterAttached(BubblePSys, BuildingMesh, NAME_None, BubblePSysLocation);
	if(BubbleComponent) AttachedParticleSystems.Add(BubbleComponent);
}

void AShieldMonolith::SpawnBeamEmitter_Implementation(UParticleSystem* PSysToSpawn, AShieldMonolith* SourceMonolith, AShieldMonolith* TargetMonolith)
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

bool AShieldMonolith::SpawnBeamEmitter_Validate(UParticleSystem* PSysToSpawn, AShieldMonolith* SourceMonolith, AShieldMonolith* TargetMonolith)
{
	return true;
}

void AShieldMonolith::DestroyConnectedBeamEmitters_Implementation()
{
	for (UParticleSystemComponent* PSys : AttachedParticleSystems) {
		if (PSys) { PSys->DeactivateSystem(); PSys->DestroyComponent(); }
	}
}

bool AShieldMonolith::DestroyConnectedBeamEmitters_Validate()
{
	return true;
}

USceneComponent* AShieldMonolith::GetBeamSocket()
{
	if (BeamPSysSocket) return BeamPSysSocket;
	return nullptr;
}

float AShieldMonolith::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health <= 0.0f) {
		DestroyConnectedBeamEmitters();
	}

	return DamageAmount;
}

TArray<AShieldMonolith*> AShieldMonolith::FindMonolithsInRange(float Range, TArray<AActor*> ActorsToIgnore)
{
	TArray<AActor*> IgnoreArray = ActorsToIgnore;
	TArray<FHitResult> SphereHits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + FVector(0, 0, 1), Range, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);
	TArray<AShieldMonolith*> WallsInRange;
	for (FHitResult Hit : SphereHits) {
		if (!WallsInRange.Contains(Hit.GetActor()) && Hit.GetActor()->IsA(AShieldMonolith::StaticClass()) && Hit.GetActor() != this) {
			float Distance = ((Hit.GetActor()->GetActorLocation()) - this->GetActorLocation()).Size();
			WallsInRange.Add(Cast<AShieldMonolith>(Hit.GetActor()));
		}
	}

	return WallsInRange;
}