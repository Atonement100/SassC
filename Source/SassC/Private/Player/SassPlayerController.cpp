// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SassPlayerController.h"

#include "Gamemode/Sassilization/SassGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/SassPlayer.h"

ASassPlayerController::ASassPlayerController()
{
	
}

void ASassPlayerController::BeginPlay()
{
	Super::BeginPlay();

	this->SassGameManager = Cast<ASassGameState>(GetWorld()->GetGameState())->GetGameManager();
}

bool ASassPlayerController::IsSpawnableRequestValid()
{
	return SassGameManager->IsValidBuildLocation();
}

bool ASassPlayerController::RequestSpawnable()
{
	ASassPlayer* SassPlayer = Cast<ASassPlayer>(this->GetPawn());
	TArray<TEnumAsByte<EObjectTypeQuery>> StaticObjectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
	StaticObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	const TArray<AActor*> RaycastIgnore;
	float BaseEyeHeight = SassPlayer->BaseEyeHeight;
	
	FHitResult InitRaycastHit;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(), SassPlayer->GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
		SassPlayer->GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
		UKismetMathLibrary::GetForwardVector(this->GetControlRotation()) * 10000.0f,
		StaticObjectTypes, true, RaycastIgnore, EDrawDebugTrace::ForOneFrame, InitRaycastHit, true);

	FVector InitialHit = InitRaycastHit.Location;
	
	return SassGameManager->RequestBuildingSpawn(this, SassPlayer->SelectedSpawnableClass,
		InitialHit, FRotator());
}

bool ASassPlayerController::RequestBuildingSpawn()
{
	ASassPlayer* SassPlayer = Cast<ASassPlayer>(this->GetPawn());
	TArray<TEnumAsByte<EObjectTypeQuery>> StaticObjectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
	StaticObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	const TArray<AActor*> RaycastIgnore;
	float BaseEyeHeight = SassPlayer->BaseEyeHeight;
	
	FHitResult InitRaycastHit;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(), SassPlayer->GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
		SassPlayer->GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
		UKismetMathLibrary::GetForwardVector(this->GetControlRotation()) * 10000.0f,
		StaticObjectTypes, true, RaycastIgnore, EDrawDebugTrace::ForOneFrame, InitRaycastHit, true);

	FVector InitialHit = InitRaycastHit.Location + 50.0f;
	
	return SassGameManager->RequestBuildingSpawn(this, SassPlayer->SelectedSpawnableClass,
		InitialHit, FRotator(0, 0, 0));
}
