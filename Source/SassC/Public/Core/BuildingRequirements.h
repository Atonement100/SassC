#pragma once

#include "Gamemode/Sassilization/TypeOfEntity.h"
#include "BuildingRequirements.generated.h"

USTRUCT(BlueprintType)
struct FBuildingRequirement
{
	GENERATED_USTRUCT_BODY()
	
	inline static int NOT_TRACKED = -1;
	
	static FBuildingRequirement AmountOnly(const int32 NewAmountRequired)
	{
		FBuildingRequirement Requirement = FBuildingRequirement();
		Requirement.AmountRequired = NewAmountRequired;
		return Requirement;
	}

	static FBuildingRequirement LevelOnly(const int32 NewLevelRequired)
	{
		FBuildingRequirement Requirement = FBuildingRequirement();
		Requirement.LevelRequired = NewLevelRequired;
		return Requirement;
	}
	
	FBuildingRequirement(): AmountRequired(NOT_TRACKED), LevelRequired(NOT_TRACKED) {}

	FBuildingRequirement(const int32 AmountRequired, const int32 LevelRequired):
		AmountRequired(AmountRequired), LevelRequired(LevelRequired) {}	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LevelRequired;

	bool ShouldTrackAmount() const { return AmountRequired > -1; }
	bool ShouldTrackLevel() const { return LevelRequired > -1; }
	FString ToString() const { return "[AmountRequired: " + FString::FromInt(AmountRequired)
		+ ", LevelRequired: " + FString::FromInt(LevelRequired) + "]"; }
};

USTRUCT(BlueprintType)
struct FBuildingRequirements
{
	GENERATED_USTRUCT_BODY()

	FBuildingRequirements(){}

	FBuildingRequirements(const TMap<ETypeOfEntity, FBuildingRequirement>& Required)
	{
		this->BuildingsRequired = Required;
	}
	
	FBuildingRequirements(const ETypeOfEntity BuildingType, const FBuildingRequirement BuildingRequirement)
	{
		AddRequirement(BuildingType, BuildingRequirement);
	}

	void AddRequirement(const ETypeOfEntity BuildingType, const FBuildingRequirement BuildingRequirement)
	{
		BuildingsRequired.Add(BuildingType, BuildingRequirement);
	}

	bool IsEntityRequired(const ETypeOfEntity EntityToCheck) const
	{
		return this->BuildingsRequired.Contains(EntityToCheck);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETypeOfEntity, FBuildingRequirement> BuildingsRequired = TMap<ETypeOfEntity, FBuildingRequirement>();
};