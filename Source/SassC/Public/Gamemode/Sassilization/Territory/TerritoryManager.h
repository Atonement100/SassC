// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GameFramework/Actor.h"
#include "Gamemode/Sassilization/Territory/EmpireBorderData.h"
#include "TerritoryManager.generated.h"

class ATerritoryVisual;
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

	AGraphNode* GetNearestNode(FVector Location, float SearchRadius) const;
	UFUNCTION(BlueprintCallable)
	bool IsLocationInTerritory(FVector Location, uint8 EmpireId) const;
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void ServerUpdateTerritories();
	void Test_ColorTerritoryBorderNodes();

protected:
	UPROPERTY(EditAnywhere)
	ANodeManager* NodeManager;
	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<FEmpireBorderData> TerritoryBorders;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ATerritoryVisual> TerritoryVisualClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<ATerritoryVisual*> TerritoryVisuals;
};
