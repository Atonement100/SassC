// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "SassGameManager.generated.h"

class ABuildingManager;
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

protected: 
	UPROPERTY(Replicated, EditAnywhere, Category = "Gamemode")
	AEmpireManager* EmpireManager;
	UPROPERTY(Replicated, EditAnywhere, Category = "Gamemode")
	ATerritoryManager* TerritoryManager;
	UPROPERTY(Replicated, EditAnywhere, Category = "Gamemode")
	ABuildingManager* BuildingManager;
	
public:
	AEmpireManager* GetEmpireManager() const;
	void SetEmpireManager(AEmpireManager* NewEmpireManager);
	ATerritoryManager* GetTerritoryManager() const;
	void SetTerritoryManager(ATerritoryManager* NewTerritoryManager);
	ABuildingManager* GetBuildingManager() const;
	void SetBuildingManager(ABuildingManager* NewBuildingManager);


};
