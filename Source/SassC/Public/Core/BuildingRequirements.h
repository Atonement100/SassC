#pragma once


#include "BuildingType.h"
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

	FBuildingRequirements(const TMap<EBuildingType, FBuildingRequirement> Required)
	{
		this->Required = Required;
	}
	
	FBuildingRequirements(const EBuildingType BuildingType, const FBuildingRequirement BuildingRequirement)
	{
		AddRequirement(BuildingType, BuildingRequirement);
	}

	void AddRequirement(const EBuildingType BuildingType, const FBuildingRequirement BuildingRequirement)
	{
		Required.Add(BuildingType, BuildingRequirement);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBuildingType, FBuildingRequirement> Required = TMap<EBuildingType, FBuildingRequirement>();
};