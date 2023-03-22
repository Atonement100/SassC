// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/Territory/TerritoryManager.h"

#include "TimerManager.h"
#include "Async/Async.h"
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

void ATerritoryManager::BeginPlay()
{
	Super::BeginPlay();

	// Only tick on server
	this->SetActorTickEnabled(GetLocalRole() == ROLE_Authority);
}

void ATerritoryManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bNeedToProcessTerritoryUpdate)
	{
		PropagateTerritoryUpdate();
		bNeedToProcessTerritoryUpdate = false;
	}
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

void ATerritoryManager::PropagateTerritoryUpdate()
{
	const ASassGameManager* GameManager = GetWorld()->GetGameState<ASassGameState>()->GetGameManager();
	
	TArray<ATerritoryVisual*> TempTerritoryVisuals = TArray<ATerritoryVisual*>();
	int RibbonId = 0;
	for (FEmpireBorderData BorderData : TerritoryBorders)
	{
		RibbonId++;
		FTransform FinalTransform = FTransform(BorderData.BorderNodes[0]->GetActorLocation()); 
		ATerritoryVisual* NewTerritoryVisual = GetWorld()->SpawnActorDeferred<ATerritoryVisual>(TerritoryVisualClass,
			FinalTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		UE_LOG(Sassilization, Display, TEXT("BorderNode count: %d"), BorderData.BorderNodes.Num())
		
		NewTerritoryVisual->MulticastSetSplinePoints(TArray<AActor*>(BorderData.BorderNodes),
			GameManager->GetEmpireManager()->GetEmpireById(BorderData.EmpireId)->GetColor(), RibbonId);
		
		NewTerritoryVisual->FinishSpawning(FinalTransform);
		TempTerritoryVisuals.Add(NewTerritoryVisual);
	}

	UE_LOG(Sassilization, Display, TEXT("Spline count: %d"), TempTerritoryVisuals.Num())
	
	for (const ATerritoryVisual* TerritoryVisual : TempTerritoryVisuals)
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

void ATerritoryManager::UpdateTerritoriesAsyncDelegate(TArray<AActor*> Buildings)
{
	const double StartTime = FPlatformTime::Seconds();
	
	TArray<FTerritoryInfo> Origins = TArray<FTerritoryInfo>();
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

	const double EndTime = FPlatformTime::Seconds();

	UE_LOG(Sassilization, Display, TEXT("Flooding took %f seconds"), EndTime - StartTime)
}

void ATerritoryManager::ServerUpdateTerritories_Implementation()
{
	UE_LOG(Sassilization, Display, TEXT("Trying to update territories"))

	if (GetWorldTimerManager().IsTimerActive(TerritoryUpdateTimeoutHandle) || bTerritoryUpdateInProgress.test_and_set())
	{
		UE_LOG(Sassilization, Display, TEXT("Territory Update In Progress or can't yet be updated."))

		if (GetWorldTimerManager().IsTimerActive(TerritoryUpdateSoonHandle))
		{
			UE_LOG(Sassilization, Display, TEXT("A territory update is already queued."))
			return;
		}
		
		const FTimerDelegate UpdateSoonTimerDelegate = FTimerDelegate().CreateLambda([this]
		{
			UE_LOG(Sassilization, Display, TEXT("Trying to update again after a delay.."))
			ServerUpdateTerritories();
		});

		UE_LOG(Sassilization, Display, TEXT("Queueing a territory update to execute after 1 second.."))
		GetWorldTimerManager().SetTimer(TerritoryUpdateSoonHandle, UpdateSoonTimerDelegate, 1.0f, false);
		return;
	}

	const FTimerDelegate TerritoryTimeoutResetDelegate = FTimerDelegate().CreateLambda([]
	{
		UE_LOG(Sassilization, Display, TEXT("Territory can be updated again."))
	});
	GetWorldTimerManager().SetTimer(TerritoryUpdateTimeoutHandle, TerritoryTimeoutResetDelegate, 0.3f, false);

	TArray<AActor*> Buildings;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildingBase::StaticClass(), Buildings);

	TFuture<void> TaskFuture = Async(EAsyncExecution::Thread, [this, Buildings]()
	{
		this->UpdateTerritoriesAsyncDelegate(Buildings);
	}, [this]()
	{
		this->bNeedToProcessTerritoryUpdate = true;
		this->bTerritoryUpdateInProgress.clear();
	});
}

bool ATerritoryManager::ServerUpdateTerritories_Validate()
{
	return true;
}
