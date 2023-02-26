// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/BuildingManager.h"

bool UBuildingManager::CanBuild(UEmpire* Empire, EBuildingType BuildingType, bool IgnoreCost) const
{
	if (!IsValid(Empire))
	{
		return false;
	}

	// todo check building attributes and cost, need to factor that out of building base itself to do it though

	// todo check building requirements
	
	return true;
}
