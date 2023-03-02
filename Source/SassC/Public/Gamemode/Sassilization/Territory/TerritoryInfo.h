// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "GraphNode.h"
#include "TerritoryInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTerritoryInfo
{
	GENERATED_USTRUCT_BODY()

	FTerritoryInfo(): EmpireId(0), ParentGraphNode(nullptr), Influence(0)
	{
	}

	FTerritoryInfo(const uint8 EmpireId, AGraphNode* ParentGraphNode, float Influence):
		EmpireId(EmpireId), ParentGraphNode(ParentGraphNode), Influence(Influence)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 EmpireId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGraphNode* ParentGraphNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Influence;
};
