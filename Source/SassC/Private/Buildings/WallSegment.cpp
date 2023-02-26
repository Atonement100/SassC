// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/WallSegment.h"
#include "SassC.h"
#include "Net/UnrealNetwork.h"
#include "Buildings/Wall.h"

AWallSegment::AWallSegment()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWallSegment::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BuildingCollision->SetVisibility(false);
	BuildingCollision->SetHiddenInGame(true);
}

void AWallSegment::BeginPlay()
{
	Super::BeginPlay();
}

void AWallSegment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if (PreviewActive) { PreviewUpgrade(); }
	//else if (!DelayReset && ResetRequired) { ResetPreview(); }
	//DelayReset = false;	
}

void AWallSegment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWallSegment, LeftConnection);
	DOREPLIFETIME(AWallSegment, RightConnection);
	DOREPLIFETIME(AWallSegment, DamageLevel);
}

void AWallSegment::HideMesh()
{
	GetMesh(0)->SetVisibility(false);
	ResetRequired = true;
	DelayReset = true;
}

void AWallSegment::ShowMesh()
{
	GetMesh(0)->SetVisibility(true);
	ResetRequired = false;
}

float AWallSegment::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health <= 0.0f)
	{
		if (LeftConnection)
		{
			TryRemove(LeftConnection, true);
			if (LeftConnection->IsA(AWallSegment::StaticClass()))
			{
				Cast<AWallSegment>(LeftConnection)->ChangeMesh(false);
			}
		}
		if (RightConnection)
		{
			TryRemove(RightConnection, false);
			if (RightConnection->IsA(AWallSegment::StaticClass()))
			{
				Cast<AWallSegment>(RightConnection)->ChangeMesh(true);
			}
		}

		this->LeftConnection = nullptr;
		this->RightConnection = nullptr;
	}

	return DamageAmount;
}

void AWallSegment::TryRemove(ABuildingBase* RemoveFrom, bool IsLeftConnection)
{
	if (AWall* WallTower = Cast<AWall>(RemoveFrom))
	{
		WallTower->ConnectedWalls.Remove(this);
	}
	else if (IsLeftConnection)
	{
		if (AWallSegment* Left = Cast<AWallSegment>(RemoveFrom))
		{
			if (Left->RightConnection) Left->RightConnection = nullptr;
		}
	}
	else if (!IsLeftConnection)
	{
		if (AWallSegment* Right = Cast<AWallSegment>(RemoveFrom))
		{
			if (Right->LeftConnection) Right->LeftConnection = nullptr;
		}
	}
}

void AWallSegment::ChangeMesh_Implementation(bool LeftConnectionDestroyed)
{
	NetChangeMesh(LeftConnectionDestroyed);
}

bool AWallSegment::ChangeMesh_Validate(bool LeftConnectionDestroyed)
{
	return true;
}

void AWallSegment::NetChangeMesh_Implementation(bool LeftConnectionDestroyed)
{
	if (DamageLevel == 0)
	{
		if (!LeftConnectionDestroyed)
		{
			if (AvailableBuildingMeshes.IsValidIndex(1))
			{
				Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[1]));
				//Rotate crumbling side towards side without a wall
				ActiveBuildingMesh->SetRelativeRotation(FRotator(0, 180.0f, 0));
			}
		}
		else if (LeftConnectionDestroyed)
		{
			if (AvailableBuildingMeshes.IsValidIndex(1))
			{
				Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[1]));
				//Shouldn't need to rotate, but for safety purposes.
				ActiveBuildingMesh->SetRelativeRotation(FRotator::ZeroRotator);
			}
		}
		DamageLevel++;
	}
	else if (DamageLevel == 1)
	{
		//Crumbling on both sides, no need to rotate
		if (AvailableBuildingMeshes.IsValidIndex(2))
		{
			Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[2]));
		}
		DamageLevel++;
	}
}

bool AWallSegment::NetChangeMesh_Validate(bool LeftConnectionDestroyed)
{
	return true;
}

int AWallSegment::TryConnection(ABuildingBase* Connection, TArray<ABuildingBase*>& ConnectedBldgs, int8 Depth,
                                bool TryLeft)
{
	if (!Connection || Depth == 4) return Depth;
	if (Connection && Connection->IsA(AWallSegment::StaticClass()))
	{
		Connection->HideMesh();
		Connection->DelayReset = true;
		Connection->ResetRequired = true;
	}
	else
	{
		return Depth;
	}
	ABuildingBase* NextConnection;
	AWallSegment* ThisSegment = Cast<AWallSegment>(Connection);
	if (TryLeft && ThisSegment->LeftConnection != nullptr) { NextConnection = ThisSegment->LeftConnection; }
	else if (!TryLeft && ThisSegment->RightConnection != nullptr) { NextConnection = ThisSegment->RightConnection; }
	else { return Depth; }

	return TryConnection(NextConnection, ConnectedBldgs, Depth + 1, TryLeft);
}

void AWallSegment::PreviewUpgrade_Implementation()
{
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

void AWallSegment::ResetPreview_Implementation()
{
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

void AWallSegment::NetUpgradeBuilding_Implementation()
{
	ResetRequired = false;
}

bool AWallSegment::NetUpgradeBuilding_Validate()
{
	return true;
}

void AWallSegment::UpgradeBuilding_Implementation()
{
	NetUpgradeBuilding();
}

bool AWallSegment::UpgradeBuilding_Validate()
{
	return true;
}
