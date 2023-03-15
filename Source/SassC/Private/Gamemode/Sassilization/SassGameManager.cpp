// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/SassGameManager.h"

#include "Core/EntityInterface.h"
#include "Gamemode/Sassilization/EntityManager.h"
#include "Gamemode/Sassilization/EmpireManager.h"
#include "Gamemode/Sassilization/Territory/TerritoryInfo.h"
#include "Gamemode/Sassilization/Territory/TerritoryManager.h"
#include "Net/UnrealNetwork.h"

ASassGameManager::ASassGameManager()
{
}

void ASassGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASassGameManager, EmpireManager);
	DOREPLIFETIME(ASassGameManager, TerritoryManager);
	DOREPLIFETIME(ASassGameManager, EntityManager);
}


bool ASassGameManager::IsValidSpawnLocation(APlayerController* Player, AActor* ActorToCheck, FVector Location, FRotator Rotator)
{
	return EntityManager->IsValidSpawnLocation(Player, ActorToCheck, Location, Rotator, GetTerritoryManager());
}

AActor* ASassGameManager::RequestGhostSpawn(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location, FRotator Rotator) const
{
	return EntityManager->SpawnGhost(Player, BuildingToSpawn, Location, Rotator);
}

bool ASassGameManager::RequestBuildingSpawn(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location, FRotator Rotator)
{
	EntityManager->SpawnEntity(Player, BuildingToSpawn, Location, Rotator, GetTerritoryManager());
	return true;
}

AGraphNode* ASassGameManager::GetClosestNode(const FVector Location, const float SearchRadius) const
{
	return this->GetTerritoryManager()->GetNearestNode(Location, SearchRadius);
}

void ASassGameManager::UpdateTerritories() const 
{
	this->TerritoryManager->ServerUpdateTerritories();
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

AEntityManager* ASassGameManager::GetEntityManager() const
{
	return EntityManager;
}

void ASassGameManager::SetEntityManager(AEntityManager* NewEntityManager)
{
	this->EntityManager = NewEntityManager;
}

