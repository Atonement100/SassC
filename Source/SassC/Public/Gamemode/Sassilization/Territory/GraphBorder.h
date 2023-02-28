// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GraphBorder.generated.h"

class AGraphNode;

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API UGraphBorder : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"))
	AGraphNode* Head;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"))
	AGraphNode* Tail;

public:
	AGraphNode* GetHead() const;
	void SetHead(AGraphNode* NewHead);
	AGraphNode* GetTail() const;
	void SetTail(AGraphNode* NewTail);
};
