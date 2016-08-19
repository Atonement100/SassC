// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "UnrealNetwork.h"
#include "wall.h"
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
	if (PreviewActive) { PreviewUpgrade(); GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Cyan, UKismetStringLibrary::Conv_IntToString(GatePreviewArray.Num())); }
	else if (!DelayReset && ResetRequired) { ResetPreview(); }
	DelayReset = false;	
}

void AwallSegment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AwallSegment, LeftConnection);
	DOREPLIFETIME(AwallSegment, RightConnection);
}

float AwallSegment::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health <= 0.0f) {
		if (LeftConnection) {
			TryRemove(LeftConnection, true);
		}
		if (RightConnection) {
			TryRemove(RightConnection, false);
		}
	
	this->LeftConnection = nullptr;
	this->RightConnection = nullptr;

	}
	
	return DamageAmount;
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

void AwallSegment::TryRemove(AbuildingBase* RemoveFrom, bool IsLeftConnection) {
	if (Awall* WallTower = Cast<Awall>(RemoveFrom)) {
		WallTower->ConnectedWalls.Remove(this);
	}
	else if (IsLeftConnection) {
		if (AwallSegment* Left = Cast<AwallSegment>(LeftConnection)) {
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, Left->RightConnection->GetName());
			Left->RightConnection = nullptr;
			if (Left->RightConnection == nullptr) {
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, "is nullptr");
			}

		}
	}
	else if (!IsLeftConnection) {
		if (AwallSegment* Right = Cast<AwallSegment>(RightConnection)) {
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, Right->LeftConnection->GetName());
			Right->LeftConnection = nullptr;
			if (Right->LeftConnection == nullptr) {
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, "is nullptr");
			}

		}
	}
}

int AwallSegment::TryConnection(AbuildingBase* Connection, TArray<AbuildingBase*> &ConnectedBldgs, int8 Depth, bool TryLeft) {
	if (!Connection || Depth == 4) return Depth;
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, Connection->GetName());
	if (Connection && Connection->IsA(AwallSegment::StaticClass())) {
		Connection->ColorBldg(FLinearColor::White, false);
		Connection->DelayReset = true;
		Connection->ResetRequired = true;
		if(!ConnectedBldgs.Contains(Connection)) ConnectedBldgs.Add(Connection);
	}
	else {
		return Depth;
	}
	AbuildingBase* NextConnection;
	AwallSegment* ThisSegment = Cast<AwallSegment>(Connection);
	if (TryLeft && ThisSegment->LeftConnection != nullptr) { NextConnection = ThisSegment->LeftConnection; }
	else if (!TryLeft && ThisSegment->RightConnection != nullptr) { NextConnection = ThisSegment->RightConnection; }
	else { return Depth; }

	return TryConnection(NextConnection, ConnectedBldgs, Depth + 1, TryLeft);
}

void AwallSegment::PreviewUpgrade_Implementation() {
	ResetRequired = true;
	PreviewActive = false;
	GatePreviewArray.Empty();
	GatePreviewArray.Add(this);

	ColorBldg(FLinearColor::White, false);
	int8 Depth = 1;

	if (LeftConnection) Depth = TryConnection(LeftConnection, GatePreviewArray, Depth, true);
	int8 LeftDepth = Depth;
	if (RightConnection && Depth < 4) Depth = TryConnection(RightConnection, GatePreviewArray, Depth, false);
	int8 RightDepth;
	LeftDepth == Depth ? RightDepth = 1 : RightDepth = Depth;

	if (Depth < 4) { 
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "Gate Not Possible"); 
		return; 
	}

	if (AwallSegment* LeftmostSegment = Cast<AwallSegment>(GatePreviewArray[LeftDepth - 1])) {
		if (AbuildingBase* TestSegment = LeftmostSegment->LeftConnection) {
			if (TestSegment->IsA(AwallSegment::StaticClass())) {
				TestSegment->ColorBldg(FLinearColor::Red);
				TestSegment->DelayReset = true;
				TestSegment->ResetRequired = true;
			}
			else if (TestSegment->IsA(Awall::StaticClass())) {
				TestSegment->ColorBldg(FLinearColor::Green);

			}
		}
	}

	if (AwallSegment* RightmostSegment = Cast<AwallSegment>(GatePreviewArray[RightDepth - 1])) {
		if (AbuildingBase* TestSegment = RightmostSegment->RightConnection) {
			if (TestSegment->IsA(AwallSegment::StaticClass())) {
				TestSegment->ColorBldg(FLinearColor::Red);
				TestSegment->DelayReset = true;
				TestSegment->ResetRequired = true;
			}
			else if (TestSegment->IsA(Awall::StaticClass())) {
				TestSegment->ColorBldg(FLinearColor::Green);
;
			}
		}
	}
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