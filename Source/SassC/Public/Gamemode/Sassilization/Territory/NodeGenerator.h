// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "NodeManager.h"
#include "UObject/Object.h"
#include "NodeGenerator.generated.h"

class AGraphNode;

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API UNodeGenerator : public UObject
{
	GENERATED_BODY()

public:
	UNodeGenerator();
	
	void GenerateNodeNetwork(const FVector& StartingLocation, ANodeManager* NodeManager);

protected:
	bool GenerateNodes(ANodeManager* NodeManager);
	bool IsLocationWalkable(FVector GroundLocation, FVector ActorLocation, ANodeManager* NodeManager) const;

	bool bIsGenerating;
	int VectorSeedIndex = 0;
	TMap<FVector, AGraphNode*> SerializedVectorsToGraphNode = {};
	TArray<FVector> VectorsToSearch = {};
	int32 MaxIterations = 30000;

	UPROPERTY()
	AGraphNode* CurrentNode = nullptr;

	float NodeSpacingDistance = SassGamemodeConstants::GetGraphNodeSpacingDistance();
	FVector OffGroundTraceOffset = FVector(0, 0, NodeSpacingDistance / 2);
	TMap<EGraphNodeDirection, FVector> NodeDirectionToDisplacement = {
		{EGraphNodeDirection::North, 		FVector(0, NodeSpacingDistance, 0)},
		{EGraphNodeDirection::East, 		FVector(NodeSpacingDistance, 0, 0)},
		{EGraphNodeDirection::South, 		FVector(0, -NodeSpacingDistance, 0)},
		{EGraphNodeDirection::West, 		FVector(-NodeSpacingDistance, 0, 0)},
		{EGraphNodeDirection::Northeast, 	FVector(NodeSpacingDistance, NodeSpacingDistance, 0)},
		{EGraphNodeDirection::Northwest, 	FVector(-NodeSpacingDistance, NodeSpacingDistance, 0)},
		{EGraphNodeDirection::Southeast, 	FVector(NodeSpacingDistance, -NodeSpacingDistance, 0)},
		{EGraphNodeDirection::Southwest, 	FVector(-NodeSpacingDistance, -NodeSpacingDistance, 0)},
	};

	float IncrementAlongPath = 3.81f;
};
