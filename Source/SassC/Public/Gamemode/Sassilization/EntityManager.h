// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GameFramework/Actor.h"
#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "Buildings/Data/BuildingData.h"
#include "EntityManager.generated.h"

class ATerritoryManager;
class AEmpire;

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API AEntityManager : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Buildings")
	bool CanAfford(AEmpire* Empire, ETypeOfEntity EntityType, bool IgnoreCost = false) const;
	bool AreCornersValid(TArray<FVector> CornerLocations,
	                     double& MaxHeight, double& MinHeight) const;
	bool CanFit(FHitResult PlayerToLocTraceResult, FBox EntityBoundingBox, FRotator Rotation, bool bCheckWalls,
	            bool bFoundation) const;
	AActor* SpawnGhost(APlayerController* Player, ETypeOfEntity EntityToSpawn, FVector Location, FRotator Rotator) const;
	UFUNCTION(BlueprintCallable)
	void SpawnEntity(APlayerController* Player, ETypeOfEntity EntityToSpawn, FVector TargetLocation, FRotator Rotator, ATerritoryManager* TerritoryManager);
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AActor> GetClassForEntityType(const ETypeOfEntity TypeOfEntity) const;
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetEntitiesOfTypeInSphere(ETypeOfEntity TypeOfEntity, FVector Location, float Radius) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETypeOfEntity, TSubclassOf<AActor>> EntityTypeToClass;
};
