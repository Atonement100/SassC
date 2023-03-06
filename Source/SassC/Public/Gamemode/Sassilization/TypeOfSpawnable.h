#pragma once

#include "UObject/Class.h"

UENUM(BlueprintType)
enum class ETypeOfSpawnable : uint8
{
	Building UMETA(DisplayName = "Building"),
	Unit UMETA(DisplayName = "Unit")
};

UENUM(BlueprintType)
enum class ETypeOfBuilding : uint8
{
	City UMETA(DisplayName = "City"),
	Workshop UMETA(DisplayName = "Workshop"),
	Tower UMETA(DisplayName = "Tower"),
	Wall UMETA(DisplayName = "Wall Tower"),
	Gate UMETA(DisplayName = "Gate"),
	ShieldMono UMETA(DisplayName = "Shield Monolith"),
	Shrine UMETA(DisplayName = "Shrine"),
};

UENUM(BlueprintType)
enum class ETypeOfUnit : uint8
{
	Soldier UMETA(DisplayName = "Soldier"),
	Archer UMETA(DisplayName = "Archer"),
	Scallywag UMETA(DisplayName = "Scallywag"),
	Ballista UMETA(DisplayName = "Ballista"),
	Catapult UMETA(DisplayName = "Catapult")
};