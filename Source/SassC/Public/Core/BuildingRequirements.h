#pragma once


#include "BuildingType.h"
#include "Gamemode/Sassilization/TypeOfSpawnable.h"
#include "BuildingRequirements.generated.h"

USTRUCT(BlueprintType)
struct FBuildingRequirement
{
	GENERATED_USTRUCT_BODY()

	FBuildingRequirement(): AmountRequired(0), LevelRequired(0) {}

	FBuildingRequirement(const uint8 AmountRequired, const uint8 LevelRequired):
		AmountRequired(AmountRequired), LevelRequired(LevelRequired) {}	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 AmountRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 LevelRequired;
};

USTRUCT(BlueprintType)
struct FBuildingRequirements
{
	GENERATED_USTRUCT_BODY()

	FBuildingRequirements(){}

	FBuildingRequirements(const TMap<ETypeOfBuilding, FBuildingRequirement> Required)
	{
		this->BuildingsRequired = Required;
	}
	
	FBuildingRequirements(const ETypeOfBuilding BuildingType, const FBuildingRequirement BuildingRequirement)
	{
		AddRequirement(BuildingType, BuildingRequirement);
	}

	void AddRequirement(const ETypeOfBuilding BuildingType, const FBuildingRequirement BuildingRequirement)
	{
		BuildingsRequired.Add(BuildingType, BuildingRequirement);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETypeOfBuilding, FBuildingRequirement> BuildingsRequired = TMap<ETypeOfBuilding, FBuildingRequirement>();
};