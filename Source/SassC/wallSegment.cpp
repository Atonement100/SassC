// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "UnrealNetwork.h"
#include "wall.h"
#include "gate.h"
#include "Kismet/KismetStringLibrary.h"
#include "wallSegment.h"

AwallSegment::AwallSegment() {
	PrimaryActorTick.bCanEverTick = true;

	DamageOneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Damage One Mesh"));
	DamageOneMesh->AttachTo(RootComponent);
	DamageTwoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Damage Two Mesh"));
	DamageTwoMesh->AttachTo(RootComponent);
}

void AwallSegment::PostInitializeComponents() {
	Super::PostInitializeComponents();

	//These are to include the additional meshes and materials in the dynamic material array, parent only adds one mesh and material by default.
	BldgMeshMaterialDynamic.Add(DamageOneMesh->CreateDynamicMaterialInstance(0, DamageOneMesh->GetMaterial(0)));
	BldgMeshMaterialDynamic.Add(DamageTwoMesh->CreateDynamicMaterialInstance(0, DamageTwoMesh->GetMaterial(0))); 
}

void AwallSegment::BeginPlay() {
	Super::BeginPlay();
}

void AwallSegment::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	//if (PreviewActive) { PreviewUpgrade(); }
	//else if (!DelayReset && ResetRequired) { ResetPreview(); }
	//DelayReset = false;	
}

void AwallSegment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AwallSegment, LeftConnection);
	DOREPLIFETIME(AwallSegment, RightConnection);
	DOREPLIFETIME(AwallSegment, DamageLevel);
}

void AwallSegment::HideMesh() {
	BuildingMesh->SetVisibility(false);
	ResetRequired = true;
	DelayReset = true;
}

void AwallSegment::ShowMesh() {
	BuildingMesh->SetVisibility(true);
	ResetRequired = false;
}

float AwallSegment::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

void AwallSegment::TryRemove(AbuildingBase* RemoveFrom, bool IsLeftConnection) {
	if (Awall* WallTower = Cast<Awall>(RemoveFrom)) {
		WallTower->ConnectedWalls.Remove(this);
	}
	else if (IsLeftConnection) {
		if (AwallSegment* Left = Cast<AwallSegment>(RemoveFrom)) {
			Left->RightConnection = nullptr;
			Left->ChangeMesh();
		}
	}
	else if (!IsLeftConnection) {
		if (AwallSegment* Right = Cast<AwallSegment>(RemoveFrom)) {
			Right->LeftConnection = nullptr;
			Right->ChangeMesh();
		}
	}
}

void AwallSegment::ChangeMesh_Implementation(){
	if (DamageLevel == 0) {
		if (!RightConnection) {
			BuildingMesh->SetVisibility(false);
			DamageOneMesh->SetVisibility(true);
			DamageOneMesh->SetRelativeRotation(FRotator(0, 180.0f, 0)); //Rotate crumbling side towards side without a wall
		}
		else if (!LeftConnection) {
			BuildingMesh->SetVisibility(false);
			DamageOneMesh->SetVisibility(true);
			DamageOneMesh->SetRelativeRotation(FRotator::ZeroRotator); //Shouldn't need to rotate, but for safety purposes.
		}
		DamageLevel++;
	}
	else if (DamageLevel == 1) {
		DamageOneMesh->SetVisibility(false);	//Crumbling on both sides, no need to rotate
		DamageTwoMesh->SetVisibility(true);
		DamageLevel++;
	}
}

bool AwallSegment::ChangeMesh_Validate()
{
	return true;
}

int AwallSegment::TryConnection(AbuildingBase* Connection, TArray<AbuildingBase*> &ConnectedBldgs, int8 Depth, bool TryLeft) {
	if (!Connection || Depth == 4) return Depth;
	if (Connection && Connection->IsA(AwallSegment::StaticClass())) {
		Connection->HideMesh();
		Connection->DelayReset = true;
		Connection->ResetRequired = true;
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
	/*
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	ResetRequired = true;
	PreviewActive = false;
	//if(!GatePreviewArray.Contains(this)) GatePreviewArray.Add(this);
	this->HideMesh();

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
				TestSegment->HideMesh();
				//if(!GatePreviewArray.Contains(TestSegment)) GatePreviewArray.Add(TestSegment);
				TestSegment->DelayReset = true;
				TestSegment->ResetRequired = true;
				FVector Location = TestSegment->GetActorLocation();
				TempLeftWall = GetWorld()->SpawnActor(Awall::StaticClass(), &Location, &FRotator::ZeroRotator, SpawnParams);
			}
			//else if (TestSegment->IsA(Awall::StaticClass())) {
				//No additional functionality if segment is a wall
			//}
		}
	}

	if (AwallSegment* RightmostSegment = Cast<AwallSegment>(GatePreviewArray[RightDepth - 1])) {
		if (AbuildingBase* TestSegment = RightmostSegment->RightConnection) {
			if (TestSegment->IsA(AwallSegment::StaticClass())) {
				TestSegment->HideMesh();
				//if (!GatePreviewArray.Contains(TestSegment)) GatePreviewArray.Add(TestSegment);
				TestSegment->DelayReset = true;
				TestSegment->ResetRequired = true;
				FVector Location = TestSegment->GetActorLocation();
				TempRightWall = GetWorld()->SpawnActor(Awall::StaticClass(), &Location, &FRotator::ZeroRotator, SpawnParams);
			}
			//else if (TestSegment->IsA(Awall::StaticClass())) {
				//No additional functionality if segment is a wall
			//}
		}
	}
	*/
}

void AwallSegment::ResetPreview_Implementation() {
	/*
	for (AbuildingBase* Wall : GatePreviewArray) {
		Wall->ShowMesh();
		ResetRequired = false;
	}
	if (TempLeftWall)TempLeftWall->Destroy();
	if (TempRightWall)TempRightWall->Destroy();
	//GatePreviewArray.Empty();
	//GatePreviewArray.Shrink();
	*/
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


