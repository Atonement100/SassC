// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SassGameStatus.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ESassGameStatus : uint8
{
	PreGame,
	GameActive,
	PostGame
};
