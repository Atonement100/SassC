// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GameFramework/Actor.h"
#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "Buildings/Data/BuildingData.h"
#include "BuildingManager.generated.h"

class AEmpire;

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
	AActor* SpawnGhost(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location, FRotator Rotator);
	UFUNCTION(BlueprintCallable)
	void SpawnBuilding(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location, FRotator Rotator);
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AActor> GetClassForBuildingType(const ETypeOfEntity TypeOfBuilding);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETypeOfEntity, TSubclassOf<AActor>> BuildingTypeToClass;
};
