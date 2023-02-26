// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "GraphNode.h"
#include "GraphBorder.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API UGraphBorder : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGraphNode* Head;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGraphNode* Tail;
};
