#pragma once

#include "UObject/Class.h"

UENUM(BlueprintType)
enum class ETypeOfEntity : uint8
{
	City UMETA(DisplayName = "City"),
	Workshop UMETA(DisplayName = "Workshop"),
	Tower UMETA(DisplayName = "Tower"),
	Wall UMETA(DisplayName = "Wall Tower"),
	Gate UMETA(DisplayName = "Gate"),
	ShieldMono UMETA(DisplayName = "Shield Monolith"),
	Shrine UMETA(DisplayName = "Shrine"),
	WallSegment UMETA(DisplayName = "Wall Segment"),
	House UMETA(DisplayName = "House"),
	
	Swordsman UMETA(DisplayName = "Swordsman"),
	Archer UMETA(DisplayName = "Archer"),
	Scallywag UMETA(DisplayName = "Scallywag"),
	Ballista UMETA(DisplayName = "Ballista"),
	Catapult UMETA(DisplayName = "Catapult"),
	Peasant UMETA(DisplayName = "Peasant")
};

class FTypeOfEntity
{
public:
	static bool IsBuilding(ETypeOfEntity TypeOfSpawnable)
	{
		return TypeOfSpawnable <= ETypeOfEntity::House;
	}

	static bool IsUnit(ETypeOfEntity TypeOfSpawnable)
	{
		return !IsBuilding(TypeOfSpawnable);
	}
};
