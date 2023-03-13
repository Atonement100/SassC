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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EmpireId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGraphNode* ParentGraphNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Influence;

	FString ToString() const 
	{
		return "EmpireId: " + FString::FromInt(EmpireId) +
				" ParentGraphNode: " + (ParentGraphNode ? ParentGraphNode->GetName() : "NULL") +
				" Influence: " + FString::SanitizeFloat(Influence);
	}
};
