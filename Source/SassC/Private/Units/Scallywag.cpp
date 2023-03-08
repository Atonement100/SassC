// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Scallywag.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Effects/ProjectileSmallArrow.h"

AScallywag::AScallywag()
{
}

void AScallywag::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AddMeshRelativeLocation(GetMovementComponent()->Velocity.Size());
	if (ActiveCommandType == EProcessingCommandType::ORDER_IDLE || (ActiveCommandType ==
		EProcessingCommandType::ORDER_BUILDING && ReachedBuilding))
	{
		GetMovementComponent()->StopMovementImmediately();
	}
}

void AScallywag::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//if(GetMesh()) UnitMeshMaterialDynamic = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
}

void AScallywag::UpdateMaterial(FLinearColor PlayerColor)
{
	Super::UpdateMaterial(PlayerColor);
}

void AScallywag::Attack_Implementation(AActor* Target)
{
	if (!Target) return;
	Super::Attack_Implementation(Target);
	SpawnProjectile(Target->GetActorLocation());
}

bool AScallywag::Attack_Validate(AActor* Target)
{
	return true;
}

void AScallywag::AddMeshRelativeLocation_Implementation(float Velocity)
{
	//Should be moving and isn't too high
	if ((ActiveCommandType >= EProcessingCommandType::ORDER_UNIT && ActiveCommandType <=
		EProcessingCommandType::ORDER_STATIC_UNIT) || bIsAttacking)
	{
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(),
		                                          GetActorLocation() - FVector(0, 0, 90),
		                                          UEngineTypes::ConvertToTraceType(
			                                          ECollisionChannel::ECC_GameTraceChannel4), true, ActorsToIgnore,
		                                          EDrawDebugTrace::ForOneFrame, Hit, true))
		{
			AddActorWorldOffset(FVector(0, 0, .5));
		}
	}
		//Is too high
	else if (!UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(),
	                                                GetActorLocation() - FVector(0, 0, 33),
	                                                UEngineTypes::ConvertToTraceType(
		                                                ECollisionChannel::ECC_GameTraceChannel4), true, ActorsToIgnore,
	                                                EDrawDebugTrace::ForOneFrame, Hit, true))
	{
		//if (GetMesh()->RelativeLocation.Z > -30) {
		AddActorWorldOffset(FVector(0, 0, -.5));
	}
}

bool AScallywag::AddMeshRelativeLocation_Validate(float Velocity)
{
	return true;
}

void AScallywag::NetAddMeshRelativeLocation_Implementation(float Velocity)
{
}

bool AScallywag::NetAddMeshRelativeLocation_Validate(float Velocity)
{
	return true;
}
