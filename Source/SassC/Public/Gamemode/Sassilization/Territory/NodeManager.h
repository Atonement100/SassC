// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GraphNodeDirection.h"
#include "GameFramework/Actor.h"
#include "Gamemode/SassGamemodeConstants.h"
#include "NodeManager.generated.h"

class AGraphNode;
class UGraphBorder;
class UGraphBorderData;
struct FTerritoryInfo;
struct FEmpireBorderData;

UCLASS(Blueprintable)
class SASSC_API ANodeManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANodeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float NodeSpacingDistance = SassGamemodeConstants::GetGraphNodeSpacingDistance();
	FVector OffGroundTraceOffset = FVector(0, 0, NodeSpacingDistance / 2);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, AGraphNode*> NodesById;
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
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void ResetTerritory();
	UFUNCTION(BlueprintCallable)
	AGraphNode* GetNodeAtPosition(FVector PositionToCheck) const;
	UFUNCTION(BlueprintCallable)
	AGraphNode* FindNearestNode(const FVector Location, const float Radius = 50.0f) const;
	UFUNCTION(BlueprintCallable)
	AGraphNode* GetNodeById(const int32 NodeId);
	bool IsConnectionSameEmpire(AGraphNode* CurrentNode, EGraphNodeDirection Direction, int32 EmpireId);
	UGraphBorder* GetBorderForNodeIfValid(AGraphNode* GraphNode);
	bool IsBorderValid(const UGraphBorder* GraphBorder);
	UFUNCTION(BlueprintCallable)
	bool GetGroundHeight(const FVector Position, FVector& GroundLocation, FVector& GroundNormal);
	UFUNCTION(BlueprintCallable)
	AGraphNode* AddNode(FVector NewLocation, FVector NewNormal, EGraphNodeDirection Direction,
	                    AGraphNode* SourceNode, TMap<FVector, AGraphNode*>& TruncatedVectorsToNode, bool& ShouldUseNewNode);
	UFUNCTION(BlueprintCallable)
	AGraphNode* SpawnAndConnectGraphNode(FVector Location, FVector Normal, AGraphNode* ParentNode);
	int32 UpdateBitmaskForConnection(AGraphNode* CurrentNode, int32 EmpireId, EGraphNodeDirection NodeDirection,
	                                 EDirectionBitmask BitmaskDirection, int32 CurrentBitmask);
	void MakeConnections(TArray<UGraphBorder*>& Borders, AGraphNode* Node);
	int32 CalculateBorderBitmask(AGraphNode* Node, int32 EmpireId);
	void AddBorderConnection(TArray<UGraphBorder*>& GraphBorders, AGraphNode* Node,
		EGraphNodeDirection NextDirection, EGraphNodeDirection PreviousDirection);
	UFUNCTION(BlueprintCallable)
	void FloodTerritory(const TArray<FTerritoryInfo>& TerritoryOrigins, TArray<FEmpireBorderData>& TerritoryBordersResult);

private:
	inline static EDirectionBitmask NO = EDirectionBitmask::North;
	inline static EDirectionBitmask NE = EDirectionBitmask::North;
	inline static EDirectionBitmask EA = EDirectionBitmask::North;
	inline static EDirectionBitmask SE = EDirectionBitmask::North;
	inline static EDirectionBitmask SO = EDirectionBitmask::North;
	inline static EDirectionBitmask WE = EDirectionBitmask::North;
	inline static EDirectionBitmask NW = EDirectionBitmask::North;
	inline static EDirectionBitmask SW = EDirectionBitmask::North;
};
