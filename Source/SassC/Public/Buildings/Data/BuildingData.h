// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include <initializer_list>

#include "CoreMinimal.h"
#include "Core/BuildingRequirements.h"
#include "Engine/DataAsset.h"
#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "BuildingData.generated.h"

class UStreamableRenderAsset;

/**
 * 
 */
UCLASS(BlueprintType)
class SASSC_API UBuildingData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UStreamableRenderAsset*> AvailableBuildingMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Influence;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int BuildTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int> InitialHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int IronCost;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int FoodCost;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int GoldCost;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int GoldValueOnDestroy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int GoldBonusOnDestroy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool CanPlayerSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool CanSpawnFoundation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FBuildingRequirements> LevelRequirements = TArray(std::initializer_list<FBuildingRequirements>(
		{FBuildingRequirements(ETypeOfEntity::City, FBuildingRequirement(1, 0))}));
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> AttackDamage;
};
