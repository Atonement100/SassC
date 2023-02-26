// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "NodeManager.h"
#include "TerritoryManager.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API UTerritoryManager : public UObject
{
	GENERATED_BODY()

public:
	bool IsLocationInTerritory(FVector Location, uint8 EmpireId) const;

protected:
	UPROPERTY(VisibleAnywhere)
	ANodeManager* NodeManager;
};
