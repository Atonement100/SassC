// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/Territory/TerritoryManager.h"

#include "Buildings/BuildingBase.h"
#include "Buildings/City.h"
#include "Gamemode/Sassilization/Territory/GraphNode.h"
#include "Kismet/GameplayStatics.h"

#include "Kismet/KismetStringLibrary.h"
#include "Net/UnrealNetwork.h"

ATerritoryManager::ATerritoryManager()
{
	bReplicates = true;
}

void ATerritoryManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATerritoryManager, TerritoryBorders);
}

bool ATerritoryManager::IsLocationInTerritory(const FVector Location, const uint8 EmpireId) const
{
	const AGraphNode* NearestNode = NodeManager->FindNearestNode(Location, 1.0f);
	
	UE_LOG(LogTemp, Display, TEXT("Found Node %s at %s with empire %d while querying for Empire %d"),
		*GetNameSafe(NearestNode), *UKismetStringLibrary::Conv_VectorToString(Location), NearestNode->GetEmpireId(), EmpireId);

	return NearestNode->GetEmpireId() == EmpireId;
}

void ATerritoryManager::UpdateTerritories_Implementation()
{
	TArray<FTerritoryInfo> Origins = TArray<FTerritoryInfo>();
	TArray<AActor*> Buildings;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACity::StaticClass(), Buildings);
	for (const AActor* Building : Buildings)
	{
		if (!IsValid(Building))
		{
			continue;
		}

		const ABuildingBase* BuildingBase = Cast<ABuildingBase>(Building);

		if (BuildingBase->IsDestroyed())
		{
			continue;
		}

		Origins.Add(BuildingBase->GetTerritoryInfo());
	}
	
	TArray<FEmpireBorderData> NewTerritoryBorders = TArray<FEmpireBorderData>();
	this->NodeManager->FloodTerritory(Origins, NewTerritoryBorders);

	this->TerritoryBorders = NewTerritoryBorders;
}
