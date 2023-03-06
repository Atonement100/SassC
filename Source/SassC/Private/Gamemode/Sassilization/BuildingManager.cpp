// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/BuildingManager.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"

bool ABuildingManager::CanBuild(AEmpire* Empire, EBuildingType BuildingType, bool IgnoreCost) const
{
	if (!IsValid(Empire))
	{
		return false;
	}

	// todo check building attributes and cost, need to factor that out of building base itself to do it though

	// todo check building requirements
	
	return true;
}

void ABuildingManager::BuildBuilding(APlayerController* Player, ETypeOfBuilding BuildingToSpawn, FVector Location, FRotator Rotator)
{
	// ActorToSpawn->GetDefaultObject(true);
	const TSubclassOf<ABuildingBase> ActorToSpawn = GetClassForBuildingType(BuildingToSpawn);
	
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	GetWorld()->SpawnActor(ActorToSpawn, &Location, &Rotator, SpawnParams);
}

TSubclassOf<ABuildingBase> ABuildingManager::GetClassForBuildingType(const ETypeOfBuilding TypeOfBuilding)
{
	if (!this->BuildingTypeToClass.Contains(TypeOfBuilding))
	{
		UE_LOG(LogTemp, Warning, TEXT("No class configured for building type %s"), *UEnum::GetValueAsString(TypeOfBuilding))
	}
	
	return this->BuildingTypeToClass[TypeOfBuilding];
}
