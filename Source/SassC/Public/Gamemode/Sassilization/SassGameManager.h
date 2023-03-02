// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "BuildingManager.h"
#include "EmpireManager.h"
#include "Territory/TerritoryManager.h"
#include "UObject/Object.h"
#include "SassGameManager.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ASassGameManager : public AActor
{
	GENERATED_BODY()

public:
	ASassGameManager();
	
	UFUNCTION(BlueprintCallable)
	bool IsValidBuildLocation();

	UFUNCTION(BlueprintCallable)
	bool RequestBuildingSpawn(APlayerController* Player, TSubclassOf<AActor> ActorToSpawn, FVector Location, FRotator Rotator);

protected: 
	UPROPERTY(EditAnywhere, Category = "Gamemode")
	AEmpireManager* EmpireManager;
	UPROPERTY(EditAnywhere, Category = "Gamemode")
	ATerritoryManager* TerritoryManager;
	UPROPERTY(EditAnywhere, Category = "Gamemode")
	ABuildingManager* BuildingManager;
	
public:
	AEmpireManager* GetEmpireManager() const;
	void SetEmpireManager(AEmpireManager* NewEmpireManager);
	ATerritoryManager* GetTerritoryManager() const;
	void SetTerritoryManager(ATerritoryManager* NewTerritoryManager);
	ABuildingManager* GetBuildingManager() const;
	void SetBuildingManager(ABuildingManager* NewBuildingManager);


};
