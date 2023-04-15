// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Territory/NodeGenerator.h"

#include "Core/ChannelDefines.h"
#include "Gamemode/Sassilization/Territory/GraphNode.h"
#include "Kismet/GameplayStatics.h"

UNodeGenerator::UNodeGenerator()
{
}

void UNodeGenerator::GenerateNodeNetwork(const FVector& StartingLocation, ANodeManager* NodeManager)
{
	TArray<AActor*> ExistingGraphNodes;
	UGameplayStatics::GetAllActorsOfClass(NodeManager->GetWorld(), AGraphNode::StaticClass(), ExistingGraphNodes);

	if (!ExistingGraphNodes.IsEmpty())
	{
		UE_LOG(Sassilization, Warning, TEXT("GraphNodes already exist in the world. Please remove any nodes before generating new ones."))
		return;
	}

	NodeManager->ResetNodeNetwork();
	
	bIsGenerating = true;

	FHitResult InitialHit;
	UKismetSystemLibrary::LineTraceSingle(NodeManager->GetWorld(), StartingLocation, StartingLocation - FVector(0, 0, 100),
		UEngineTypes::ConvertToTraceType(ECC_LEVEL_MESH), true, {}, EDrawDebugTrace::None, InitialHit, true);

	UE_LOG(Sassilization, Display, TEXT("Seeding vectors to search with %s"), *InitialHit.Location.ToString())
	
	VectorsToSearch.Add(InitialHit.Location);

	const double StartTime = FPlatformTime::Seconds();

	while (!GenerateNodes(NodeManager)) {}
	
	const double EndTime = FPlatformTime::Seconds();
	UE_LOG(Sassilization, Display, TEXT("Completed Node Generation in %f seconds"), EndTime - StartTime)
	// UE_LOG(Sassilization, Display, TEXT("Num vectors to search [%d], Num final vectors [%d], Num serialized vectors [%d], Final vector seed index [%d]")
	// 	VectorsToSearch.Num(), Final)
}

bool UNodeGenerator::GenerateNodes(ANodeManager* NodeManager)
{
	while (NodeManager->GetNumNodesKnown() < MaxIterations && bIsGenerating)
	{
		if (!CurrentNode)
		{
			if (VectorSeedIndex >= VectorsToSearch.Num())
			{
				bIsGenerating = false;
				UE_LOG(Sassilization, Display, TEXT("Stopping node generation because no more vectors to search were present after %d"), VectorSeedIndex)
				continue;
			}

			// A hit here means there is a valid location for a node to spawn
			FHitResult NewNodeHitResult;
			FVector TraceStart = VectorsToSearch[VectorSeedIndex] + FVector(0, 0, 20.f);
			FVector TraceEnd = TraceStart - FVector(0, 0, 50.f);
			bool bWasHit = UKismetSystemLibrary::LineTraceSingle(NodeManager->GetWorld(), TraceStart, TraceEnd,
				UEngineTypes::ConvertToTraceType(ECC_LEVEL_MESH), true, {},
				EDrawDebugTrace::None, NewNodeHitResult, true);

			VectorSeedIndex++;

			if (!bWasHit || NodeManager->GetNodeAtPosition(NewNodeHitResult.Location))
			{
				UE_LOG(Sassilization, Warning, TEXT("Continuing because either there was no hit [%s] or there was a node found at location %s"),
					(bWasHit ? TEXT("TRUE") : TEXT("FALSE")), *NewNodeHitResult.Location.ToString())
				continue;
			}

			CurrentNode = NodeManager->SpawnAndConnectGraphNode(NewNodeHitResult.Location, NewNodeHitResult.Normal, CurrentNode);
			SerializedVectorsToGraphNode.Add(NewNodeHitResult.Location, CurrentNode);
			//
			// return true;
		}

		UE_LOG(Sassilization, Display, TEXT("Evaluating node %s"), *CurrentNode->GetName())
		
		for (const EGraphNodeDirection Direction : TEnumRange<EGraphNodeDirection>())
		{
			const int DirectionIndex = StaticCast<int>(Direction);

			if (CurrentNode->GetHasVisitedAtIndex(DirectionIndex)) continue;

			CurrentNode->SetVisited(DirectionIndex, true);

			FVector GroundLocation, GroundNormal;

			if (!NodeManager->GetGroundHeight(CurrentNode->GetActorLocation() + NodeDirectionToDisplacement[Direction], GroundLocation, GroundNormal))
			{
				UE_LOG(Sassilization, Warning, TEXT("Ground not found around actor location %s"), *CurrentNode->GetActorLocation().ToString())
				return false;
			}
			
			if (!IsLocationWalkable(GroundLocation, CurrentNode->GetActorLocation(), NodeManager))
			{
				UE_LOG(Sassilization, Warning, TEXT("Slope is not walkable around actor location %s"), *CurrentNode->GetActorLocation().ToString());
				return false;
			}

			TMap<FVector, AGraphNode*> TruncatedVectorsToNode = TMap<FVector, AGraphNode*>();
			bool bShouldUseNewNode = false;
			AGraphNode* NewNode = NodeManager->AddNode(GroundLocation, GroundNormal, Direction, CurrentNode, TruncatedVectorsToNode, bShouldUseNewNode);

			if (!bShouldUseNewNode)
			{
				continue;
			}

			CurrentNode = NewNode;

			UE_LOG(Sassilization, Display, TEXT("Set node as current: %s"), *CurrentNode->ToString())
			return false;
		}

		CurrentNode = CurrentNode->GetParentNode();
	}

	UE_LOG(Sassilization, Display, TEXT("Stopped generating nodes. Vector seed index was %d. Max iterations allowed was %d. IsGenerating is %hs"),
		VectorSeedIndex, MaxIterations, (bIsGenerating ? "TRUE" : "FALSE"))
	return true;
}

bool UNodeGenerator::IsLocationWalkable(FVector GroundLocation, FVector ActorLocation, ANodeManager* NodeManager) const
{
	//If we have a hit here, it means there was something blocking the view between the previous and tested locations
	FHitResult GroundValidHit;

	if (UKismetSystemLibrary::LineTraceSingle(NodeManager->GetWorld(), CurrentNode->GetActorLocation() + OffGroundTraceOffset, GroundLocation + OffGroundTraceOffset,
		UEngineTypes::ConvertToTraceType(ECC_LEVEL_MESH), true, {}, EDrawDebugTrace::None, GroundValidHit, true))
	{
		UE_LOG(Sassilization, Warning, TEXT("Trace hit while validating point-to-point visibility. Hit %s"), *GroundValidHit.ToString())
		return false;
	}

	float GroundZ = GroundLocation.Z;
	float ActorZ = ActorLocation.Z;

	if (GroundZ - ActorZ > NodeSpacingDistance + 0.1f || GroundZ - ActorZ > 57.15f) // In the blueprint, I left a comment saying "TODO DEATHDROP".. not sure why, maybe just for the constant name.
	{
		UE_LOG(Sassilization, Warning, TEXT("Node too much higher than the other or drop would be deadly for units"))
		return false;
	}

	float DistanceAlongPath = IncrementAlongPath;
	FVector PathDelta = GroundLocation - ActorLocation;
	FVector PointAlongPath;
	bool bWalkableAlongPathTestDone = false;

	while (!bWalkableAlongPathTestDone)
	{
		if (DistanceAlongPath >= NodeSpacingDistance)
		{
			PointAlongPath = GroundLocation;
			bWalkableAlongPathTestDone = true;
		}
		else
		{
			PointAlongPath = ActorLocation + PathDelta * (DistanceAlongPath / NodeSpacingDistance);
		}

		FVector PathGroundLocation, PathGroundNormal;
		bool bFoundGroundOnPath = NodeManager->GetGroundHeight(PointAlongPath, PathGroundLocation, PathGroundNormal);

		if (!bFoundGroundOnPath)
		{
			UE_LOG(Sassilization, Warning, TEXT("Not walkable because no ground found at a point along the path"))
			return false;
		}

		if (PathGroundNormal.Z < 0.65f)
		{
			UE_LOG(Sassilization, Warning, TEXT("%s not walkable because of ground normal %s"), *PathGroundLocation.ToString(), *PathGroundNormal.ToString())
			return false;
		}

		DistanceAlongPath += IncrementAlongPath;
	}
	
	return true;
}
