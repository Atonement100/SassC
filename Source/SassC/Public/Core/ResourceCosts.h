// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ResourceCosts.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FResourceCosts
{
	GENERATED_BODY()

	FResourceCosts(): Iron(10000), Food(10000), Gold(10000), Supply(10000)
	{
	}

	FResourceCosts(const float Iron, const float Food, const float Gold, const float Supply = 0):
		Iron(Iron), Food(Food), Gold(Gold), Supply(Supply)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Iron;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Food;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Supply;
};
