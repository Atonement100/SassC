﻿// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/SassGameManager.h"

#include "Net/UnrealNetwork.h"

ASassGameManager::ASassGameManager()
{
}

void ASassGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASassGameManager, EmpireManager);
	DOREPLIFETIME(ASassGameManager, TerritoryManager);
	DOREPLIFETIME(ASassGameManager, BuildingManager);
}


bool ASassGameManager::IsValidBuildLocation()//UEmpire* Empire, )
{
	//BuildingManager->CanBuild()
	return true;
}

AActor* ASassGameManager::RequestGhostSpawn(APlayerController* Player, ETypeOfEntity BuildingToSpawn,
	FVector Location, FRotator Rotator) const
{
	return BuildingManager->SpawnGhost(Player, BuildingToSpawn, Location, Rotator);
}

bool ASassGameManager::RequestBuildingSpawn(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location, FRotator Rotator)
{
	BuildingManager->SpawnBuilding(Player, BuildingToSpawn, Location, Rotator);
	return true;
}

AEmpireManager* ASassGameManager::GetEmpireManager() const
{
	return EmpireManager;
}

void ASassGameManager::SetEmpireManager(AEmpireManager* NewEmpireManager)
{
	this->EmpireManager = NewEmpireManager;
}

ATerritoryManager* ASassGameManager::GetTerritoryManager() const
{
	return TerritoryManager;
}

void ASassGameManager::SetTerritoryManager(ATerritoryManager* NewTerritoryManager)
{
	this->TerritoryManager = NewTerritoryManager;
}

ABuildingManager* ASassGameManager::GetBuildingManager() const
{
	return BuildingManager;
}

void ASassGameManager::SetBuildingManager(ABuildingManager* NewBuildingManager)
{
	this->BuildingManager = NewBuildingManager;
}

