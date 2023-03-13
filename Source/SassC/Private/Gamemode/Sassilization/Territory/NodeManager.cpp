// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Territory/NodeManager.h"

#include "Engine/World.h"
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

// Called every frame
void ANodeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AGraphNode* ANodeManager::FindNearestNode(const FVector Location, const float Radius) const
{
	AGraphNode* NearestNode = nullptr;
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	const bool bWasOverlapping = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Location, Radius,
		TraceObjectTypes, AGraphNode::StaticClass(), TArray<AActor*>(), OverlappingActors);

	if (!bWasOverlapping)
	{
		UE_LOG(LogTemp, Warning, TEXT("No nodes found in range."));
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

void ANodeManager::FloodTerritory_Implementation(const TArray<FTerritoryInfo>& TerritoryOrigins, TArray<FEmpireBorderData>& TerritoryBordersResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Called unimplemented function"))
}

