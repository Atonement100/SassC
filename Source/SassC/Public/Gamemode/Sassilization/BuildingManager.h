// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "Buildings/BuildingBase.h"
#include "Empire.h"
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
	bool CanBuild(UEmpire* Empire, EBuildingType BuildingType, bool IgnoreCost) const;
	UFUNCTION(BlueprintCallable)
	void BuildBuilding(APlayerController* Player, TSubclassOf<AActor> ActorToSpawn, FVector Location, FRotator Rotator);
	// UFUNCTION(BlueprintCallable)
	//
	
};
