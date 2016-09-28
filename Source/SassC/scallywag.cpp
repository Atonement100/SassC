// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Kismet/KismetSystemLibrary.h"
#include "projectileSmallArrow.h"
#include "scallywag.h"

Ascallywag::Ascallywag() {
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere->SetWorldScale3D(FVector(ScallywagAttackRange / SelectionSphereScaleMod));
}

void Ascallywag::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	AddMeshRelativeLocation(GetMovementComponent()->Velocity.Size());
	if (!((ActiveCommandType >= EProcessingCommandType::ORDER_UNIT && ActiveCommandType <= EProcessingCommandType::ORDER_STATIC_UNIT) || ReachedBuilding)) {
		GetMovementComponent()->StopMovementImmediately();
	}
}

void Ascallywag::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//if(GetMesh()) UnitMeshMaterialDynamic = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
}

void Ascallywag::UpdateMaterial(FLinearColor PlayerColor)
{
	Super::UpdateMaterial(PlayerColor);
}

void Ascallywag::Attack_Implementation(AActor * Target)
{
	if (!Target) return;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Scallywag attack");

	Super::Attack_Implementation(Target);
	SpawnProjectile(Target->GetActorLocation());
}

bool Ascallywag::Attack_Validate(AActor * Target)
{
	return true;
}

void Ascallywag::SpawnProjectile_Implementation(FVector TargetLocation)
{
	//This and related functions are not expressed in unitBase because each projectile-shooting 
	//unit has the potential for very unreleated projectile logic.
	FVector TargetDisplacement = (TargetLocation - this->GetActorLocation());

	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	const FVector Location = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket"));
	const FRotator Rotation = TargetDisplacement.Rotation();

	AActor* Projectile = GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation, SpawnParams);
	FVector Velocity = Cast<AprojectileSmallArrow>(Projectile)->MovementComponent->Velocity;
	if (Projectile) Projectile->SetLifeSpan((TargetDisplacement).Size() / Velocity.Size());
}

bool Ascallywag::SpawnProjectile_Validate(FVector TargetLocation)
{
	return true;
}

void Ascallywag::AddMeshRelativeLocation_Implementation(float Velocity) {
	//Should be moving and isn't too high
	if ((ActiveCommandType >= EProcessingCommandType::ORDER_UNIT && ActiveCommandType <= EProcessingCommandType::ORDER_STATIC_UNIT) || IsAttacking) {
		if (UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), GetActorLocation(), GetActorLocation() - FVector(0, 0, 90), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4), true, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, Hit, true)) {
			AddActorWorldOffset(FVector(0, 0, .5));
		}
	}
	//Is too high
	else if (!UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), GetActorLocation(), GetActorLocation() - FVector(0, 0, 33), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4), true, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, Hit, true)) {//if (GetMesh()->RelativeLocation.Z > -30) {
		AddActorWorldOffset(FVector(0, 0, -.5));
	}
}

bool Ascallywag::AddMeshRelativeLocation_Validate(float Velocity){
	return true;
}

void Ascallywag::NetAddMeshRelativeLocation_Implementation(float Velocity) {

}

bool Ascallywag::NetAddMeshRelativeLocation_Validate(float Velocity) {
	return true;
}