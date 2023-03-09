// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GameFramework/Actor.h"
#include "Gamemode/Sassilization/Territory/EmpireBorderData.h"
#include "TerritoryManager.generated.h"

class ANodeManager;

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API ATerritoryManager : public AActor
{
	GENERATED_BODY()

public:
	ATerritoryManager();
	
	UFUNCTION(BlueprintCallable)
	bool IsLocationInTerritory(FVector Location, uint8 EmpireId) const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateTerritories();
	void UpdateTerritories_Implementation();

protected:
	UPROPERTY(EditAnywhere)
	ANodeManager* NodeManager;
	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<FEmpireBorderData> TerritoryBorders;
};
