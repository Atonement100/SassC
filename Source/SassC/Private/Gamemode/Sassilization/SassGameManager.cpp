// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/SassGameManager.h"

ASassGameManager::ASassGameManager()
{
}


bool ASassGameManager::IsValidBuildLocation()//UEmpire* Empire, )
{
	//BuildingManager->CanBuild()
	return true;
}

bool ASassGameManager::RequestBuildingSpawn(APlayerController* Player, TSubclassOf<AActor> ActorToSpawn, FVector Location, FRotator Rotator)
{
	BuildingManager->BuildBuilding(Player, ActorToSpawn, Location, Rotator);
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

