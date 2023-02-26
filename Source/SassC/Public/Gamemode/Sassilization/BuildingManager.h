// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "Buildings/BuildingBase.h"
#include "Empire.h"
#include "BuildingManager.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API UBuildingManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Buildings")
	bool CanBuild(UEmpire* Empire, EBuildingType BuildingType, bool IgnoreCost) const;
	
protected:
	
};
