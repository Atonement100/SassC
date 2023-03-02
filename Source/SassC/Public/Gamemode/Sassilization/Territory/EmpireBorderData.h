// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "GraphNode.h"
#include "EmpireBorderData.generated.h"

USTRUCT(BlueprintType)
struct FEmpireBorderData
{
	GENERATED_USTRUCT_BODY()

	FEmpireBorderData(): EmpireId(0)
	{
	}

	FEmpireBorderData(const uint8 EmpireId, const TArray<AGraphNode*> BorderNodes):
		EmpireId(EmpireId), BorderNodes(BorderNodes) {}	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 EmpireId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGraphNode*> BorderNodes;
};
