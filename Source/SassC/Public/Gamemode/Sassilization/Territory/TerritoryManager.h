﻿// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "NodeManager.h"
#include "TerritoryManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API ATerritoryManager : public AActor
{
	GENERATED_BODY()

public:
	bool IsLocationInTerritory(FVector Location, uint8 EmpireId) const;

protected:
	UPROPERTY(EditAnywhere)
	ANodeManager* NodeManager;
};