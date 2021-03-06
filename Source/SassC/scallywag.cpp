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
	if (ActiveCommandType == EProcessingCommandType::ORDER_IDLE || (ActiveCommandType == EProcessingCommandType::ORDER_BUILDING && ReachedBuilding)) {
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
	Super::Attack_Implementation(Target);
	SpawnProjectile(Target->GetActorLocation());
}

bool Ascallywag::Attack_Validate(AActor * Target)
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