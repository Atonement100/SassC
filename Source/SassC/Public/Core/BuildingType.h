#pragma once


#include "BuildingType.generated.h"

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
	City,
	Gate,
	ShieldMonolith,
	Shrine,
	Tower,
	Wall,
	WallSegment,
	Workshop
};
