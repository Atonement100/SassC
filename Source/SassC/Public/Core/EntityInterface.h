// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "UObject/Interface.h"
#include "EntityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UEntityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SASSC_API IEntityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual ETypeOfEntity GetTypeOfEntity() {return ETypeOfEntity::City;};
};
