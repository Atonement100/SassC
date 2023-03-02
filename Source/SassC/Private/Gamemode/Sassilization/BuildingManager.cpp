// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/BuildingManager.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"

bool ABuildingManager::CanBuild(UEmpire* Empire, EBuildingType BuildingType, bool IgnoreCost) const
{
	if (!IsValid(Empire))
	{
		return false;
	}

	// todo check building attributes and cost, need to factor that out of building base itself to do it though

	// todo check building requirements
	
	return true;
}

void ABuildingManager::BuildBuilding(APlayerController* Player, TSubclassOf<AActor> ActorToSpawn, FVector Location, FRotator Rotator)
{
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	
	GetWorld()->SpawnActor(ActorToSpawn, &Location, &Rotator, SpawnParams);
}
