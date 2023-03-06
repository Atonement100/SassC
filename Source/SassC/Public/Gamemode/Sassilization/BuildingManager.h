// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "Buildings/BuildingBase.h"
#include "Gamemode/Sassilization/Empire.h"
#include "Gamemode/Sassilization/TypeOfSpawnable.h"
#include "Buildings/Data/BuildingData.h"
#include "BuildingManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API ABuildingManager : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Buildings")
	bool CanBuild(AEmpire* Empire, EBuildingType BuildingType, bool IgnoreCost) const;
	UFUNCTION(BlueprintCallable)
	void BuildBuilding(APlayerController* Player, ETypeOfBuilding BuildingToSpawn, FVector Location, FRotator Rotator);
	UFUNCTION(BlueprintCallable)
	TSubclassOf<ABuildingBase> GetClassForBuildingType(const ETypeOfBuilding TypeOfBuilding);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETypeOfBuilding, TSubclassOf<ABuildingBase>> BuildingTypeToClass;
};
