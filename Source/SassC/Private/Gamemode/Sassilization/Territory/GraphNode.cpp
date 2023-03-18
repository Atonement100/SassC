// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Territory/GraphNode.h"

#include "Gamemode/Sassilization/Territory/GraphBorderData.h"
#include "Gamemode/Sassilization/Territory/NodeManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AGraphNode::AGraphNode()
{
	this->bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;
	Connections.SetNum(8, false);
	IdConnections.SetNum(8, false);
	HasVisited.SetNum(8, false);
}

void AGraphNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGraphNode, AStarFScore)
}

ANodeManager* AGraphNode::GetOrFindNodeManager()
{
	if (!this->NodeManager)
	{
		UE_LOG(Sassilization, Warning, TEXT("Needed to find NodeManager"))
		this->NodeManager = Cast<ANodeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ANodeManager::StaticClass()));
	}

	return this->NodeManager;
}

// Called when the game starts or when spawned
void AGraphNode::BeginPlay()
{
	Super::BeginPlay();
}

int32 AGraphNode::GetEmpireId() const
{
	return UKismetMathLibrary::Round(this->GetAStarFScore());
}

void AGraphNode::SetConnection(AGraphNode* NodeToConnect, EGraphNodeDirection Direction)
{
	this->IdConnections[static_cast<int>(Direction)] = NodeToConnect->Id;
}

AGraphNode* AGraphNode::GetConnection(EGraphNodeDirection Direction)
{
	AGraphNode* Result = NodeManager->GetNodeById(this->IdConnections[static_cast<int>(Direction)]);
	
	// UE_LOG(Sassilization, Warning, TEXT("Node %s tried to get direction %d with id %d and it returned %s"),
		// *this->GetName(), Direction, this->IdConnections[static_cast<int>(Direction)], (Result? *Result->GetName() : TEXT("null")))
	return Result;
}

void AGraphNode::SetVisited(const int Index, const bool HasBeenVisited)
{
	this->HasVisited[Index] = HasBeenVisited;
}

bool AGraphNode::GetHasVisitedAtIndex(int Index)
{
	return HasVisited[Index];
}

FVector AGraphNode::GetLocation()
{
	return this->GetActorLocation();
}

FVector AGraphNode::GetNormal()
{
	return this->Normal;
}

void AGraphNode::SetAStarStatus(AGraphNode* NewAStarParentNode, float NewAStarFScore, float NewAStarGScore)
{
	this->AStarParentNode = IsValid(NewAStarParentNode) ? NewAStarParentNode : nullptr;
	this->AStarFScore = NewAStarFScore;
	this->AStarGScore = NewAStarGScore;
}

void AGraphNode::ClearBorderData()
{
	this->BorderData = nullptr;
}

bool AGraphNode::HasValidBorderData()
{
	if (!IsValid(BorderData))
	{
		return false;
	}

	if (!IsValid(BorderData->GetBorder()))
	{
		return false;
	}

	const UGraphBorder* GraphBorder = BorderData->GetBorder();
	return (IsValid(GraphBorder->GetHead()) || IsValid(GraphBorder->GetTail()));
}

void AGraphNode::SetBorder(UGraphBorder* Border)
{
	this->BorderData->SetBorder(Border);
}

UGraphBorder* AGraphNode::GetBorder()
{
	return this->BorderData->GetBorder();
}

void AGraphNode::SetBorderNext(AGraphNode* GraphNode)
{
	this->BorderData->SetNext(GraphNode);
}

AGraphNode* AGraphNode::GetBorderNext()
{
	return this->BorderData->GetNext();
}

void AGraphNode::SetBorderPrev(AGraphNode* GraphNode)
{
	this->BorderData->SetPrevious(GraphNode);
}

AGraphNode* AGraphNode::GetBorderPrev()
{
	return this->BorderData->GetPrevious();
}

float AGraphNode::GetAStarFScore() const
{
	return this->AStarFScore;
}

// Called every frame
void AGraphNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
