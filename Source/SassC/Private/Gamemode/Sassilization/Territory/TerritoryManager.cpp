// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/Territory/TerritoryManager.h"

#include "Buildings/BuildingBase.h"
#include "Buildings/City.h"
#include "Components/SplineComponent.h"
#include "Gamemode/Sassilization/Empire.h"
#include "Gamemode/Sassilization/EmpireManager.h"
#include "Gamemode/Sassilization/SassGameManager.h"
#include "Gamemode/Sassilization/SassGameState.h"
#include "Gamemode/Sassilization/Territory/GraphNode.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemode/Sassilization/Territory/NodeManager.h"
#include "Gamemode/Sassilization/Territory/TerritoryVisual.h"
#include "Kismet/KismetStringLibrary.h"
#include "Net/UnrealNetwork.h"

ATerritoryManager::ATerritoryManager()
{
	bReplicates = true;
}

void ATerritoryManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATerritoryManager, TerritoryBorders);
}

AGraphNode* ATerritoryManager::GetNearestNode(FVector Location, float SearchRadius) const
{
	return NodeManager->FindNearestNode(Location, SearchRadius);
}

bool ATerritoryManager::IsLocationInTerritory(const FVector Location, const uint8 EmpireId) const
{
	const AGraphNode* NearestNode = GetNearestNode(Location, 20.f);

	if (!NearestNode)
	{
		UE_LOG(Sassilization, Warning, TEXT("No node found in range when performing territory check. Loc %s EmpireId %d"), *Location.ToString(), EmpireId)
		return false;
	}
	
	UE_LOG(Sassilization, Verbose, TEXT("Found Node %s at %s with empire %d while querying for Empire %d"),
		*GetNameSafe(NearestNode), *UKismetStringLibrary::Conv_VectorToString(Location), NearestNode->GetEmpireId(), EmpireId)

	return NearestNode->GetEmpireId() == EmpireId;
}

void ATerritoryManager::Test_ColorTerritoryBorderNodes()
{
	TArray<AActor*> AllNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGraphNode::StaticClass(), AllNodes);

	for (AActor* GraphNode : AllNodes)
	{
		Cast<AGraphNode>(GraphNode)->ColorSelf(FLinearColor::White);
	}
	
	for (FEmpireBorderData BorderData : this->TerritoryBorders)
	{
		AEmpire* Empire = GetWorld()->GetGameState<ASassGameState>()->GetGameManager()->GetEmpireManager()->GetEmpireById(BorderData.EmpireId);
		FLinearColor Color = Empire->GetColor();
		for (AGraphNode* GraphNode : BorderData.BorderNodes)
		{
			GraphNode->ColorSelf(Color);
		}
	}
}

void ATerritoryManager::ServerUpdateTerritories_Implementation()
{
	UE_LOG(Sassilization, Display, TEXT("Trying to update territories"))
	TArray<FTerritoryInfo> Origins = TArray<FTerritoryInfo>();
	TArray<AActor*> Buildings;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildingBase::StaticClass(), Buildings);
	for (const AActor* Building : Buildings)
	{
		if (!IsValid(Building))
		{
			continue;
		}

		const ABuildingBase* BuildingBase = Cast<ABuildingBase>(Building);
		
		if (!BuildingBase->IsBuilt() || BuildingBase->IsDestroyed() || BuildingBase->GetTerritoryInfo().Influence == 0.0f)
		{
			continue;
		}

		UE_LOG(Sassilization, Display, TEXT("Adding TerritoryInfo %s"), *BuildingBase->GetTerritoryInfo().ToString())
		Origins.Add(BuildingBase->GetTerritoryInfo());
	}
	
	TArray<FEmpireBorderData> NewTerritoryBorders = TArray<FEmpireBorderData>();

	UE_LOG(Sassilization, Display, TEXT("Found %d territory origins"), Origins.Num())
	
	this->NodeManager->FloodTerritory(Origins, NewTerritoryBorders);

	UE_LOG(Sassilization, Display, TEXT("Found %d new territory borders"), NewTerritoryBorders.Num())
	
	this->TerritoryBorders = NewTerritoryBorders;
	this->Test_ColorTerritoryBorderNodes();

	const ASassGameManager* GameManager = GetWorld()->GetGameState<ASassGameState>()->GetGameManager();
	
	TArray<ATerritoryVisual*> TempTerritoryVisuals = TArray<ATerritoryVisual*>();
	int Index2 = 0;
	for (FEmpireBorderData BorderData : TerritoryBorders)
	{
		Index2++;
		FTransform FinalTransform = FTransform(BorderData.BorderNodes[0]->GetActorLocation()); 
		ATerritoryVisual* NewTerritoryVisual = GetWorld()->SpawnActorDeferred<ATerritoryVisual>(TerritoryVisualClass,
			FinalTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		UE_LOG(Sassilization, Display, TEXT("BorderNode count: %d"), BorderData.BorderNodes.Num())
		
		NewTerritoryVisual->MulticastSetSplinePoints(TArray<AActor*>(BorderData.BorderNodes),
			GameManager->GetEmpireManager()->GetEmpireById(BorderData.EmpireId)->GetColor(), Index2);
		
		NewTerritoryVisual->FinishSpawning(FinalTransform);
		TempTerritoryVisuals.Add(NewTerritoryVisual);
	}

	UE_LOG(Sassilization, Display, TEXT("Spline count: %d"), TempTerritoryVisuals.Num())
	for (ATerritoryVisual* TerritoryVisual : TempTerritoryVisuals)
	{
		UE_LOG(Sassilization, Display, TEXT("Number of spline points for %s was %d"), *TerritoryVisual->SplineComponent->GetFullName(), TerritoryVisual->SplineComponent->GetNumberOfSplinePoints())
	}

	while (!this->TerritoryVisuals.IsEmpty())
	{
		const int Index = TerritoryVisuals.Num() - 1;
		this->TerritoryVisuals[Index]->Destroy();
		this->TerritoryVisuals.RemoveAt(Index);
	}
	
	this->TerritoryVisuals = TempTerritoryVisuals;
}

bool ATerritoryManager::ServerUpdateTerritories_Validate()
{
	return true;
}
