﻿// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Territory/NodeManager.h"

#include "Core/ChannelDefines.h"
#include "Gamemode/Sassilization/Territory/EmpireBorderData.h"
#include "Gamemode/Sassilization/Territory/GraphNode.h"
#include "Gamemode/Sassilization/Territory/TerritoryInfo.h"
#include "Gamemode/Sassilization/Territory/NodeGenerator.h"
#include "Engine/World.h"
#include "Gamemode/Sassilization/Territory/GraphBorderData.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


ANodeManager::ANodeManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANodeManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANodeManager::Tick(float DeltaTime)
{
	
}

void ANodeManager::ResetTerritory()
{
	UE_LOG(Sassilization, Display, TEXT("Resetting territory, %d nodes"), NodesById.Num())

	for (TPair<int32, AGraphNode*> NodeById : NodesById)
	{
		if (!NodeById.Value) continue;
		
		NodeById.Value->ResetTraversalFlags();
		NodeById.Value->SetAStarStatus(nullptr, 0, 0);
	}
}

/*
 * Note: original previously iterated through all nodes and returned if any node was within a specific tolerance of .45 * stepsize
 */
AGraphNode* ANodeManager::GetNodeAtPosition(FVector PositionToCheck) const
{
	return FindNearestNode(PositionToCheck, 10.0f);
}

AGraphNode* ANodeManager::FindNearestNode(const FVector Location, const float Radius) const
{
	AGraphNode* NearestNode = nullptr;
	TArray<AActor*> OverlappingActors;

	UE_LOG(Sassilization, Display, TEXT("Searching for actors at location %s with radius %f"), *Location.ToString(), Radius)
	
	const bool bWasOverlapping = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Location, Radius,
		{UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)},
		AGraphNode::StaticClass(), {}, OverlappingActors);

	if (!bWasOverlapping)
	{
		UE_LOG(Sassilization, Warning, TEXT("No nodes found in range around %s."), *Location.ToString());
		return NearestNode;
	}

	uint32 NearestDistance = MAX_uint32;

	for (short Index = 0; Index < OverlappingActors.Num(); Index++)
	{
		const double LocalDistance = UKismetMathLibrary::Vector_Distance(OverlappingActors[Index]->GetActorLocation(), Location);
		if (LocalDistance < NearestDistance)
		{
			NearestNode = Cast<AGraphNode>(OverlappingActors[Index]);
			NearestDistance = LocalDistance;
		}
	}

	return NearestNode;
}

AGraphNode* ANodeManager::GetNodeById(const int32 NodeId)
{
	AGraphNode** FoundNode = NodesById.Find(NodeId);

	if (!FoundNode)
	{
		UE_LOG(Sassilization, Warning, TEXT("Node with id %d not found"), NodeId)
		return nullptr;
	}

	return *FoundNode;
}

TArray<FEmpireBorderData> ANodeManager::FloodTerritory(const TArray<FTerritoryInfo>& TerritoryOrigins)
{
	ResetTerritory();

	TArray<FEmpireBorderData> EmpireBorderDataList = TArray<FEmpireBorderData>();
	
	if (TerritoryOrigins.IsEmpty())
	{
		UE_LOG(Sassilization, Warning, TEXT("No territory origins were found"))
		return EmpireBorderDataList;
	}

	TArray<AGraphNode*> OpenedNodes = TArray<AGraphNode*>();
	TArray<AGraphNode*> ClosedNodes = TArray<AGraphNode*>();
	
	for (FTerritoryInfo Origin : TerritoryOrigins)
	{
		AGraphNode* ParentNode = Origin.ParentGraphNode;
		
		if (!ParentNode)
		{
			UE_LOG(Sassilization, Warning, TEXT("TerritoryInfo ParentGraphNode invalid"))
			continue;
		}

		OpenedNodes.Add(ParentNode);
		ParentNode->SetIsOpen(true);
		ParentNode->SetAStarStatus(nullptr, Origin.EmpireId, Origin.Influence);
	}

	UE_LOG(Sassilization, Display, TEXT("%d nodes were opened."), OpenedNodes.Num())

	while (!OpenedNodes.IsEmpty())
	{
		AGraphNode* ActiveNode = OpenedNodes[0];
		ClosedNodes.Add(ActiveNode);
		ActiveNode->SetIsClosed(true);
		OpenedNodes.RemoveAt(0);

		for (const EGraphNodeDirection Direction : TEnumRange<EGraphNodeDirection>())
		{
			AGraphNode* ConnectedNode = ActiveNode->GetConnection(Direction);
			if (!ConnectedNode || ConnectedNode->IsClosed()) continue;

			const float NewAStarGScore = ActiveNode->GetAStarGScore() + FVector::Distance(ActiveNode->GetActorLocation(), ConnectedNode->GetActorLocation());

			if (NewAStarGScore <= 257.175f && (!ConnectedNode->IsOpen() || NewAStarGScore < ConnectedNode->GetAStarGScore()))
			{
				ConnectedNode->SetAStarStatus(ActiveNode, ActiveNode->GetAStarFScore(), NewAStarGScore);
				ConnectedNode->SetIsOpen(true);
				OpenedNodes.Add(ConnectedNode);
			}
		}
	}

	UE_LOG(Sassilization, Display, TEXT("%d nodes were closed."), ClosedNodes.Num())

	TArray<AGraphNode*> BorderNodes = TArray<AGraphNode*>();
	
	for (AGraphNode* ActiveNode : ClosedNodes)
	{
		for (int Index = 0; Index <= 3; Index++) // For each node direction prior to bi-cardinals
		{
			const AGraphNode* ConnectedNode = ActiveNode->GetConnection(StaticCast<EGraphNodeDirection>(Index));

			if (!ConnectedNode)
			{
				UE_LOG(Sassilization, Display, TEXT("Connection was not valid. Active node %s. Direction %d"), *ActiveNode->GetName(), Index)
			}
			else
			{
				if (ConnectedNode->GetEmpireId() == ActiveNode->GetEmpireId()) { continue; }
				if (BorderNodes.Contains(ActiveNode)) { continue; }

				BorderNodes.Add(ActiveNode);
				break;
			}
		}
	}

	TArray<UGraphBorder*> GraphBorders = TArray<UGraphBorder*>();
	
	for (AGraphNode* BorderNode : BorderNodes)
	{
		MakeConnections(GraphBorders, BorderNode);
	}
	
	for (const UGraphBorder* Border : GraphBorders)
	{
		AGraphNode* StartingBorderNode = Border->GetTail();
		AGraphNode* CurrentBorderNode = StartingBorderNode;
		AGraphNode* NextBorderNode = nullptr;
		TArray<AGraphNode*> NodesForBorder = TArray<AGraphNode*>();
		float CurrentEmpireId = 0.f;

		while (CurrentBorderNode)
		{
			CurrentEmpireId = CurrentBorderNode->GetAStarFScore();
			NodesForBorder.Add(CurrentBorderNode);

			NextBorderNode = CurrentBorderNode->GetBorderNext();

			if (CurrentBorderNode->GetBorderData())
			{
				CurrentBorderNode->ResetBorderData();
			}

			CurrentBorderNode = NextBorderNode;

			if (CurrentBorderNode == StartingBorderNode)
			{
				NodesForBorder.Add(CurrentBorderNode);
				break;
			}
		}

		FEmpireBorderData EmpireBorderData = FEmpireBorderData();
		EmpireBorderData.EmpireId = UKismetMathLibrary::Round(CurrentEmpireId);
		EmpireBorderData.BorderNodes = NodesForBorder;
		EmpireBorderDataList.Add(EmpireBorderData);
	}

	return EmpireBorderDataList;
}

void ANodeManager::GenerateNodeNetwork()
{
	UNodeGenerator* NodeGenerator = NewObject<UNodeGenerator>();
	NodeGenerator->GenerateNodeNetwork(GetActorLocation(), this);
}

void ANodeManager::ResetNodeNetwork()
{
	this->NodesById.Empty();
}

bool ANodeManager::IsConnectionSameEmpire(AGraphNode* CurrentNode, EGraphNodeDirection Direction, int32 EmpireId)
{
	AGraphNode* Connection = CurrentNode->GetConnection(Direction);

	/**
	 * In this case no connection exists. I think this diverges from the original.
	 * IsTerritory returns true if the connection does not exist, but that seemed
	 * to leave edge-of-map (e.g. against a wall) nodes as unclaimed. For straight edges,
	 * this was OK, but it (or I) didn't handle curves correctly. May revise.
	 */
	if (!IsValid(Connection))
	{
		return false;
	}

	return Connection->GetEmpireId() == EmpireId;
}

UGraphBorder* ANodeManager::GetBorderForNodeIfValid(AGraphNode* GraphNode)
{
	if (!IsValid(GraphNode))
	{
		return nullptr;
	}

	if (!GraphNode->GetBorderData())
	{
		return nullptr;
	}
	
	UGraphBorder* GraphBorder = GraphNode->GetBorder();
	
	if (!IsValid(GraphBorder))
	{
		return nullptr;
	}

	if (GraphBorder->GetHead() == GraphNode ||
		GraphBorder->GetTail() == GraphNode)
	{
		return GraphBorder;
	}

	return nullptr;
}

bool ANodeManager::IsBorderValid(const UGraphBorder* GraphBorder)
{
	if (!GraphBorder)
	{
		UE_LOG(Sassilization, Verbose, TEXT("Graph Border was not valid"))
		return false;
	}

	return GraphBorder->GetHead() && GraphBorder->GetTail();
}

bool ANodeManager::GetGroundHeight(const FVector Position, FVector& GroundLocation, FVector& GroundNormal)
{
	TArray<FHitResult> Hits = TArray<FHitResult>();
	FVector TraceTarget = FVector(Position.X, Position.Y, -9999.f);
	
	for (float Offset = 19.05; Offset < 190.5; Offset += 19.05)
	{
		FHitResult HitResult;

		bool WasHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Position + FVector(0, 0, Offset), TraceTarget,
			UEngineTypes::ConvertToTraceType(ECC_LEVEL_MESH), true, { this },
			EDrawDebugTrace::None, HitResult, true);

		if (HitResult.bStartPenetrating) { continue; }
		if (!WasHit) { continue; }
		if (!(Hits.IsEmpty() || Hits.Last().Location.Z > HitResult.Location.Z)) continue;

		Hits.Add(HitResult);
	}

	if (Hits.IsEmpty())
	{
		GroundLocation = FVector::Zero();
		GroundNormal = FVector::Zero();
		return false;
	}

	for (int Index = 0; Index < Hits.Num() - 1; Index++)
	{
		if (Hits[Index+1].Location.Z - Hits[Index].Location.Z <= 100.f) { continue; }

		GroundLocation = Hits[Index].Location;
		GroundNormal = Hits[Index].Normal;
		return true;
	}

	GroundLocation = Hits.Last().Location;
	GroundNormal = Hits.Last().Normal;
	return true;
}

AGraphNode* ANodeManager::AddNode(FVector NewLocation, FVector NewNormal, EGraphNodeDirection Direction,
	AGraphNode* SourceNode, TMap<FVector, AGraphNode*>& TruncatedVectorsToNode, bool& ShouldUseNewNode)
{
	AGraphNode* NewNode = GetNodeAtPosition(NewLocation);

	if (!NewNode)
	{
		NewNode = SpawnAndConnectGraphNode(NewLocation, NewNormal, SourceNode);
		ShouldUseNewNode = true;
	}

	if (!IsValid(NewNode))
	{
		return NewNode;
	}

	SourceNode->SetConnection(NewNode, Direction);

	EGraphNodeDirection OppositeDirection = FGraphNodeDirection::Opposite(Direction);
	NewNode->SetConnection(SourceNode, OppositeDirection);
	NewNode->SetVisited(StaticCast<uint8>(OppositeDirection), true);

	return NewNode;
}

AGraphNode* ANodeManager::SpawnAndConnectGraphNode(const FVector Location, const FVector Normal, AGraphNode* ParentNode)
{
	UE_LOG(Sassilization, Verbose, TEXT("Spawning GraphNode at %s"), *Location.ToString())
	
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
	AGraphNode* NewNode = GetWorld()->SpawnActor<AGraphNode>(AGraphNode::StaticClass(), Location, FRotator(0, 0, 0), SpawnParameters);
	NewNode->SetNormal(Normal);
	NewNode->SetParentNode(ParentNode);
	NewNode->SetNodeManager(this);
	NewNode->SetId(NodesById.Num());

	NodesById.Add(NewNode->GetId(), NewNode);

	NewNode->SetFolderPath("GraphNodes");
	
	UE_LOG(Sassilization, Display, TEXT("Spawned GraphNode %s at %s"), *NewNode->GetName(), *NewNode->GetActorLocation().ToString())
	return NewNode;
}

int32 ANodeManager::UpdateBitmaskForConnection(AGraphNode* CurrentNode, int32 EmpireId, EGraphNodeDirection NodeDirection,
	EDirectionBitmask BitmaskDirection, int32 CurrentBitmask)
{
	if (!IsConnectionSameEmpire(CurrentNode, NodeDirection, EmpireId))
	{
		return CurrentBitmask;
	}

	return CurrentBitmask | StaticCast<int32>(BitmaskDirection);
}

// After having replicated this manually in blueprints there is not an ounce of my soul
// that could handle rewriting this all manually from the original.
void ANodeManager::MakeConnections(TArray<UGraphBorder*>& Borders, AGraphNode* Node)
{
	if (!Node) return;

	const int32 EmpireId = Node->GetEmpireId();

	if (EmpireId <= 0) return;

	const int32 BorderBitmask = CalculateBorderBitmask(Node, EmpireId);

	// UE_LOG(Sassilization, Display, TEXT("BorderBitmask: %d"), BorderBitmask)
	// UE_LOG(Sassilization, Display, TEXT("BorderBitmask comparison: %d"), (NO | NE | EA | SE | SO | WE | NW))
	// UE_LOG(Sassilization, Display, TEXT("BorderBitmask equals: %d"), (SE | SO | WE | NW))
	// UE_LOG(Sassilization, Display, TEXT("BorderBitmask and: %d"), (BorderBitmask & (NO | NE | EA | SE | SO | WE | NW)))
	
	// 90 Deg. Corners
	if((BorderBitmask & (NO | NE | EA | SE | SO | WE | NW)) == (SE | SO | WE | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::East, EGraphNodeDirection::North);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | SW | WE | NW)) == (NE | EA | SO | SW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::North, EGraphNodeDirection::West);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE | NW)) == (NO | EA | SE | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::West, EGraphNodeDirection::South);
	}
	else if((BorderBitmask & (NO | NE | EA | SE | SO | SW | WE)) == (NO | NE | SW | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::South, EGraphNodeDirection::East);
	}
	// Straight diagonals
	else if((BorderBitmask & (NO | EA | SE | SO | WE | NW)) == (NO | EA))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northwest, EGraphNodeDirection::Southeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | SW | WE)) == (NO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southwest, EGraphNodeDirection::Northeast);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | WE | NW)) == (SO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southeast, EGraphNodeDirection::Northwest);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | SW | WE)) == (EA | SO))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northeast, EGraphNodeDirection::Southwest);
	}
	// Same-side diagonals
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE)) == (NO | EA | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southwest, EGraphNodeDirection::Southeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SE | SO | WE)) == (NO | SO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southeast, EGraphNodeDirection::Northeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | WE | NW)) == (EA | SO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northeast, EGraphNodeDirection::Northwest);
	}
	else if((BorderBitmask & (NO | EA | SO | SW | WE | NW)) == (NO | EA | SO))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northwest, EGraphNodeDirection::Southwest);
	}
	// Straight centers
	else if((BorderBitmask & (NO | NE | EA | SE | SO | WE)) == (NE | EA | SE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::North, EGraphNodeDirection::South);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE)) == (SE | SO | SW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::East, EGraphNodeDirection::West);
	}
	else if((BorderBitmask & (NO | WE | SO | SW | WE | NW)) == (SW | WE | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::South, EGraphNodeDirection::North);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | WE | NW)) == (NO | NE | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::West, EGraphNodeDirection::East);
	}
	// "L" connections 
	else if((BorderBitmask & (NO | NE | EA | SO | WE | NW)) == (NO | NE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northwest, EGraphNodeDirection::East);
	}
	else if((BorderBitmask & (NO | NE | EA | SE | SO | WE)) == (NE | EA))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::North, EGraphNodeDirection::Southeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SE | SO | WE)) == (EA | SE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northeast, EGraphNodeDirection::South);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE)) == (SE | SO))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::East, EGraphNodeDirection::Southwest);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE)) == (SO | SW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southeast, EGraphNodeDirection::West);
	}
	else if((BorderBitmask & (NO | EA | SO | SW | WE | NW)) == (SW | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::South, EGraphNodeDirection::Northwest);
	}
	else if((BorderBitmask & (NO | EA | SO | SW | WE | NW)) == (WE | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southwest, EGraphNodeDirection::North);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | WE | NW)) == (NO | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::West, EGraphNodeDirection::Northeast);
	}
	// "Backwards-L" connections
	else if((BorderBitmask & (NO | NE | EA | SO | SW | WE)) == (NE | EA | SO))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::North, EGraphNodeDirection::Southwest);
	}
	else if((BorderBitmask & (NO | EA | SO | SW | WE | NW)) == (NO | EA | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::West, EGraphNodeDirection::Southeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | SW | WE)) == (NO | SW | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::South, EGraphNodeDirection::Northeast);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | WE | NW)) == (SE | SO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::East, EGraphNodeDirection::Northwest);
	}
	// Reverse "L" connections
	else if((BorderBitmask & (NO | NE | EA | SO | SW | WE)) == (NO | NE | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southwest, EGraphNodeDirection::East);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | WE | NW)) == (SO | WE | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southeast, EGraphNodeDirection::North);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | SW | WE)) == (EA | SO | SW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northeast, EGraphNodeDirection::West);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | WE | NW)) == (NO | EA | SE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northwest, EGraphNodeDirection::South);
	}
	// Reverse same-side diagonals
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE)) == (SO))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southeast, EGraphNodeDirection::Southwest);
	}
	else if((BorderBitmask & (NO | NE | EA | SE | SO | WE)) == (EA))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northeast, EGraphNodeDirection::Southeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | WE | NW)) == (NO))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northwest, EGraphNodeDirection::Northeast);
	}
	else if((BorderBitmask & (NO | EA | SO | SW | WE | NW)) == (WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southwest, EGraphNodeDirection::Northwest);
	}
	// Reverse short connections
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE)) == (NO | EA | SE | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southwest, EGraphNodeDirection::South);
	}
	else if((BorderBitmask & (NO | NE | EA | SE | SO | WE)) == (NO | NE | SO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Southeast, EGraphNodeDirection::East);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | WE | NW)) == (EA | SO | WE | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northeast, EGraphNodeDirection::North);
	}
	else if((BorderBitmask & (NO | EA | SO | SW | WE | NW)) == (NO | EA | SO | SW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::Northwest, EGraphNodeDirection::West);
	}
	else if((BorderBitmask & (NO | EA | SE | SO | SW | WE)) == (NO | EA | SW | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::South, EGraphNodeDirection::Southeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SE | SO | WE)) == (NO | SE | SO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::East, EGraphNodeDirection::Northeast);
	}
	else if((BorderBitmask & (NO | NE | EA | SO | WE | NW)) == (NE | EA | SO | WE))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::North, EGraphNodeDirection::Northwest);
	}
	else if((BorderBitmask & (NO | EA | SO | SW | WE | NW)) == (NO | EA | SO | NW))
	{
		AddBorderConnection(Borders, Node, EGraphNodeDirection::West, EGraphNodeDirection::Southwest);
	}
	// Rare fail-safe case
	else if((BorderBitmask & (NO | EA | SO | WE)) == (NO | SO))
	{
		Node->SetAStarStatus( nullptr, 0, 0);
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Northeast));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::East));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Southeast));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Southwest));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::West));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Northwest));
	}
	else if((BorderBitmask & (NO | EA | SO | WE)) == (EA | WE))
	{
		Node->SetAStarStatus( nullptr, 0, 0);
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Northeast));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::North));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Southeast));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Southwest));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::South));
		MakeConnections(Borders, Node->GetConnection(EGraphNodeDirection::Northwest));
	}
	else
	{
		UE_LOG(Sassilization, Warning, TEXT("Somehow failed to make connections for graph node %s with bitmask %d"), *Node->GetName(), BorderBitmask)
	}
}

int32 ANodeManager::CalculateBorderBitmask(AGraphNode* Node, int32 EmpireId)
{
	int32 BorderBitmask = 0;

	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::North, EmpireId))		BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::North);
	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::Northeast, EmpireId)) BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::Northeast);
	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::East, EmpireId))		BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::East);
	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::Southeast, EmpireId)) BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::Southeast);
	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::South, EmpireId))		BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::South);
	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::Southwest, EmpireId)) BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::Southwest);
	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::West, EmpireId))		BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::West);
	if (IsConnectionSameEmpire(Node, EGraphNodeDirection::Northwest, EmpireId)) BorderBitmask |= StaticCast<uint8>(EDirectionBitmask::Northwest);

	return ~BorderBitmask;
}

void ANodeManager::AddBorderConnection(TArray<UGraphBorder*>& GraphBorders, AGraphNode* Node,
	EGraphNodeDirection NextDirection, EGraphNodeDirection PreviousDirection)
{
	UE_LOG(Sassilization, Display, TEXT("AddBorderConnection for %s with next dir %s and prev dir %s"), *Node->GetName(),
		*UEnum::GetValueAsString(NextDirection), *UEnum::GetValueAsString(PreviousDirection))
	
	if (!Node) return;

	UE_LOG(Sassilization, Display, TEXT("Node for this call: %s"), *Node->ToString())
	
	if (!Node->GetBorderData())
	{
		Node->ResetBorderData();
	}

	if (GetBorderForNodeIfValid(Node))
	{
		UE_LOG(Sassilization, Warning, TEXT("Border for node was somehow invalid %s"), *Node->ToString())
		return;
	}

	UGraphBorder *NextBorder = nullptr, *PreviousBorder = nullptr;

	AGraphNode* NextNode = Node->GetConnection(NextDirection);
	AGraphNode* PreviousNode = Node->GetConnection(PreviousDirection);

	if (NextNode)
	{
		if (NextNode->HasValidBorderData())
		{
			NextBorder = GetBorderForNodeIfValid(NextNode);
		}
		else
		{
			NextNode->ResetBorderData();
		}
	}

	if (PreviousNode)
	{
		if (PreviousNode->HasValidBorderData())
		{
			PreviousBorder = GetBorderForNodeIfValid(PreviousNode);
		}
		else
		{
			PreviousNode->ResetBorderData();
		}
	}

	if (!NextBorder && !PreviousBorder)
	{
		UE_LOG(Sassilization, Display, TEXT("NoBorder Node: %s."), *Node->ToString())
		
		UGraphBorder* NewBorder = NewObject<UGraphBorder>();
		NewBorder->SetHead(Node);
		NewBorder->SetTail(Node);
		Node->SetBorder(NewBorder);
		GraphBorders.Add(NewBorder);
		return;
	}
	else if (NextBorder && PreviousBorder)
	{
		if (NextBorder == PreviousBorder)
		{
			UE_LOG(Sassilization, Display, TEXT("Loop Node: %s. Next Tail %s. Next Head %s."), *Node->ToString(), *NextBorder->GetTail()->ToString(), *NextBorder->GetHead()->ToString())
			
			Node->SetBorder(NextBorder);
			Node->SetBorderNext(NextBorder->GetTail());
			NextBorder->GetTail()->SetBorderPrev(Node);
			Node->SetBorderPrev(NextBorder->GetHead());
			NextBorder->GetHead()->SetBorderNext(Node);
			NextBorder->SetHead(Node);
			NextBorder->SetTail(Node);
			return;
		}
		else
		{
			UE_LOG(Sassilization, Display, TEXT("Merge Node: %s. Next Tail %s. Next Head %s. Prev Tail %s. Prev Head %s."), *Node->ToString(),
				*NextBorder->GetTail()->ToString(), *NextBorder->GetHead()->ToString(),
				*PreviousBorder->GetTail()->ToString(), *PreviousBorder->GetHead()->ToString())

			NextBorder->GetTail()->SetBorderPrev(Node);
			Node->SetBorderNext(NextBorder->GetTail());
			NextBorder->SetTail(PreviousBorder->GetTail());
			PreviousBorder->GetHead()->SetBorderNext(Node);
			Node->SetBorderPrev(PreviousBorder->GetHead());
			PreviousBorder->GetTail()->SetBorder(NextBorder);
			NextBorder->GetTail()->SetBorder(NextBorder);

			GraphBorders.Remove(PreviousBorder);
			return;
		}
	}
	else if (NextBorder)
	{
		UE_LOG(Sassilization, Display, TEXT("NextBorder Tail: %s."), *NextBorder->GetTail()->ToString())
		NextBorder->GetTail()->SetBorderPrev(Node);
		Node->SetBorderNext(NextBorder->GetTail());
		NextBorder->SetTail(Node);
		Node->SetBorder(NextBorder);
		return;
	}
	else
	{
		UE_LOG(Sassilization, Display, TEXT("PrevBorder Head: %s"), *PreviousBorder->GetHead()->ToString())
		PreviousBorder->GetHead()->SetBorderNext(Node);
		Node->SetBorderPrev(PreviousBorder->GetHead());
		PreviousBorder->SetHead(Node);
		Node->SetBorder(PreviousBorder);
		return;
	}
}
