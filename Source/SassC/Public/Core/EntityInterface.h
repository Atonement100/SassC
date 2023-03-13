// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "BuildingRequirements.h"
#include "ResourceCosts.h"
#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "UObject/Interface.h"
#include "EntityInterface.generated.h"

class AEmpire;

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
	virtual ETypeOfEntity GetTypeOfEntity() const {return ETypeOfEntity::City;}
	virtual FResourceCosts GetResourceCosts() const {return FResourceCosts();}
	virtual FBuildingRequirements GetRequirementsForLevel(int Level) const {return FBuildingRequirements();}
	virtual FBox GetSpawnBoundingBox() const {return FBox();}
	virtual bool IsBuilding() const {return false;}
	virtual bool IsUnit() const {return false;}
	virtual bool CanPlayerSpawn() const {return true;}
	virtual void Initialize(AEmpire* NewEmpire) {}
	virtual void WhenBuilt() {}
	virtual void SetControl(AEmpire* NewEmpire) {}
	virtual AEmpire* GetEmpire() const {return nullptr;}
	virtual FVector GetSpawnOffset() {return FVector::Zero();}
};
