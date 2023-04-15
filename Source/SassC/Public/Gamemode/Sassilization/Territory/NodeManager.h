// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GraphNodeDirection.h"
#include "GameFramework/Actor.h"
#include "Gamemode/SassGamemodeConstants.h"
#include "NodeManager.generated.h"

class AGraphNode;
class UGraphBorder;
class UGraphBorderData;
class UNodeGenerator;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, AGraphNode*> NodesById;
	
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
	TArray<FEmpireBorderData> FloodTerritory(const TArray<FTerritoryInfo>& TerritoryOrigins);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void GenerateNodeNetwork();
	void ResetNodeNetwork();
	int32 GetNumNodesKnown() const {return this->NodesById.Num();}
	
private:
	inline static uint8 NO = StaticCast<uint8>(EDirectionBitmask::North);
	inline static uint8 NE = StaticCast<uint8>(EDirectionBitmask::Northeast);
	inline static uint8 EA = StaticCast<uint8>(EDirectionBitmask::East);
	inline static uint8 SE = StaticCast<uint8>(EDirectionBitmask::Southeast);
	inline static uint8 SO = StaticCast<uint8>(EDirectionBitmask::South);
	inline static uint8 SW = StaticCast<uint8>(EDirectionBitmask::Southwest);
	inline static uint8 WE = StaticCast<uint8>(EDirectionBitmask::West);
	inline static uint8 NW = StaticCast<uint8>(EDirectionBitmask::Northwest);
};
