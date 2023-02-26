// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/Territory/TerritoryManager.h"
#include "Gamemode/Sassilization/Territory/GraphNode.h"

#include "Kismet/KismetStringLibrary.h"

bool UTerritoryManager::IsLocationInTerritory(const FVector Location, const uint8 EmpireId) const
{
	const AGraphNode* NearestNode = NodeManager->FindNearestNode(Location, 1.0f);
	
	UE_LOG(LogTemp, Display, TEXT("Found Node %s at %s with empire %d while querying for Empire %d"),
		*GetNameSafe(NearestNode), *UKismetStringLibrary::Conv_VectorToString(Location), NearestNode->GetEmpireId(), EmpireId);

	return NearestNode->GetEmpireId() == EmpireId;
}
