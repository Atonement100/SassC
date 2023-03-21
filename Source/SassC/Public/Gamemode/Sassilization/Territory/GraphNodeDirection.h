#pragma once

#include "Misc/EnumRange.h"
#include "UObject/Class.h"

UENUM(BlueprintType)
enum class EGraphNodeDirection : uint8
{
	North,
	East,
	South,
	West,
	Northeast,
	Northwest,
	Southeast,
	Southwest
};
ENUM_RANGE_BY_FIRST_AND_LAST(EGraphNodeDirection, EGraphNodeDirection::North, EGraphNodeDirection::Southwest)

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true") )
enum class EDirectionBitmask : uint8
{
	None		= 0,
	North 		= 1 << 0,
	Northeast 	= 1 << 1,
	East 		= 1 << 2,
	Southeast 	= 1 << 3,
	South 		= 1 << 4,
	Southwest 	= 1 << 5,
	West 		= 1 << 6,
	Northwest 	= 1 << 7,
};
ENUM_CLASS_FLAGS(EDirectionBitmask)

class FGraphNodeDirection
{
public:
	static EGraphNodeDirection Opposite(EGraphNodeDirection Direction)
	{
		switch (Direction)
		{
		case EGraphNodeDirection::North: 		return EGraphNodeDirection::South;
		case EGraphNodeDirection::East: 		return EGraphNodeDirection::West;
		case EGraphNodeDirection::South: 		return EGraphNodeDirection::North;
		case EGraphNodeDirection::West: 		return EGraphNodeDirection::East;
		case EGraphNodeDirection::Northeast: 	return EGraphNodeDirection::Southwest;
		case EGraphNodeDirection::Northwest: 	return EGraphNodeDirection::Southeast;
		case EGraphNodeDirection::Southeast: 	return EGraphNodeDirection::Northwest;
		case EGraphNodeDirection::Southwest: 	return EGraphNodeDirection::Northeast;
		default:
			UE_LOG(Sassilization, Warning, TEXT("Called Opposite for a direction that is not configured"));
			return EGraphNodeDirection::North;
		}
	}
};