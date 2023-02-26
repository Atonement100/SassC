// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Territory/GraphNode.h"

#include "Kismet/KismetMathLibrary.h"


AGraphNode::AGraphNode()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AGraphNode::BeginPlay()
{
	Super::BeginPlay();
	
}

uint8 AGraphNode::GetEmpireId() const
{
	return UKismetMathLibrary::Round(this->GetAStarFScore());
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

