// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "UnrealNetwork.h"
#include "Kismet/KismetStringLibrary.h"
#include "wallSegment.h"

AwallSegment::AwallSegment() {
	PrimaryActorTick.bCanEverTick = true;

}

void AwallSegment::PostInitializeComponents() {
	Super::PostInitializeComponents();
	//BldgMeshMaterialDynamic = BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0));
}

void AwallSegment::BeginPlay() {
	Super::BeginPlay();
}

void AwallSegment::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (PreviewActive) { PreviewUpgrade(); }
	else if (!DelayReset && ResetRequired) { ResetPreview(); }
}

void AwallSegment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AwallSegment, LeftConnection);
	DOREPLIFETIME(AwallSegment, RightConnection);
}

/*
void AwallSegment::FixSpawnLocation_Implementation(FVector RealLocation)
{
	NetFixSpawnLocation(RealLocation);
}

bool AwallSegment::FixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

void AwallSegment::NetFixSpawnLocation_Implementation(FVector RealLocation)
{
	Role = ROLE_Authority;
	SetActorLocation(RealLocation + FVector(10));
	SetActorLocation(RealLocation);
}

bool AwallSegment::NetFixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}
*/

int AwallSegment::TryConnection(AbuildingBase* Connection, TArray<AbuildingBase*> &ConnectedBldgs, int8 Depth, bool TryLeft) {
	if (!Connection || Depth == 4) return Depth;
	if (Connection && Connection->IsA(AwallSegment::StaticClass())) {
		Connection->ColorBldg(FLinearColor::White, false);
		Connection->DelayReset = true;
		Connection->ResetRequired = true;
		ConnectedBldgs.Add(Connection);
	}
	else {
		return Depth;
	}
	AbuildingBase* NextConnection;
	AwallSegment* ThisSegment = Cast<AwallSegment>(Connection);
	if (TryLeft && ThisSegment->LeftConnection) { NextConnection = ThisSegment->LeftConnection; }
	else if (!TryLeft && ThisSegment->RightConnection) { NextConnection = ThisSegment->RightConnection; }
	else { return Depth; }

	return TryConnection(NextConnection, ConnectedBldgs, Depth + 1, TryLeft);
}

void AwallSegment::PreviewUpgrade_Implementation() {
	ColorBldg(FLinearColor::White, false);

	if (LeftConnection) TryConnection(LeftConnection, GatePreviewArray, 1, true);

	ResetRequired = true;
	PreviewActive = false;
}

void AwallSegment::ResetPreview_Implementation() {
	ColorBldg(OwningPlayerColor, false);
	for (AbuildingBase* Wall : GatePreviewArray) {
		ColorBldg(OwningPlayerColor, false);
	}
	ResetRequired = false;
}

void AwallSegment::NetUpgradeBuilding_Implementation() {

	ResetRequired = false;
}

bool AwallSegment::NetUpgradeBuilding_Validate() {
	return true;
}

void AwallSegment::UpgradeBuilding_Implementation() {
	NetUpgradeBuilding();
}

bool AwallSegment::UpgradeBuilding_Validate() {
	return true;
}