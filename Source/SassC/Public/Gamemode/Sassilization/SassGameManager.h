// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "Territory/GraphNode.h"
#include "SassGameManager.generated.h"

class AEntityManager;
class AEmpireManager;
class ATerritoryManager;

/**
 * 
 */
UCLASS()
class SASSC_API ASassGameManager : public AActor
{
	GENERATED_BODY()

public:
	ASassGameManager();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	bool IsValidBuildLocation();
	AActor* RequestGhostSpawn(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location, FRotator Rotator) const;

	UFUNCTION(BlueprintCallable)
	bool RequestBuildingSpawn(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location,
		FRotator Rotator);
	AGraphNode* GetClosestNode(FVector Location, float SearchRadius) const;
	void UpdateTerritories() const;

protected: 
	UPROPERTY(Replicated, EditAnywhere, Category = "Gamemode")
	AEmpireManager* EmpireManager;
	UPROPERTY(Replicated, EditAnywhere, Category = "Gamemode")
	ATerritoryManager* TerritoryManager;
	UPROPERTY(Replicated, EditAnywhere, Category = "Gamemode")
	AEntityManager* EntityManager;
	
public:
	AEmpireManager* GetEmpireManager() const;
	void SetEmpireManager(AEmpireManager* NewEmpireManager);
	ATerritoryManager* GetTerritoryManager() const;
	void SetTerritoryManager(ATerritoryManager* NewTerritoryManager);
	AEntityManager* GetEntityManager() const;
	void SetEntityManager(AEntityManager* NewEntityManager);


};
