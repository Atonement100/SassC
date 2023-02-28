#pragma once

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
