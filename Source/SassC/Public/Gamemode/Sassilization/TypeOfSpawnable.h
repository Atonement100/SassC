#pragma once

#include "UObject/Class.h"

UENUM(BlueprintType)
enum class ETypeOfSpawnable : uint8
{
	Building_City UMETA(DisplayName = "City"),
	Building_Workshop UMETA(DisplayName = "Workshop"),
	Building_Tower UMETA(DisplayName = "Tower"),
	Building_Wall UMETA(DisplayName = "Wall"),
	Building_Gate UMETA(DisplayName = "Gate"),
	Building_ShieldMono UMETA(DisplayName = "Shield Monolith"),
	Building_Shrine UMETA(DisplayName = "Shrine"),

	Unit_Soldier UMETA(DisplayName = "Soldier"),
	Unit_Archer UMETA(DisplayName = "Archer"),
	Unit_Scallywag UMETA(DisplayName = "Scallywag"),
	Unit_Ballista UMETA(DisplayName = "Ballista"),
	Unit_Catapult UMETA(DisplayName = "Catapult")
};