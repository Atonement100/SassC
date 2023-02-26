// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "GraphBorder.h"
#include "GraphNode.h"
#include "GraphBorderData.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API UGraphBorderData : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGraphBorder* Border;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGraphNode* Next;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGraphNode* Previous;
};
