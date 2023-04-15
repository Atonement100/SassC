// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GraphBorder.h"
#include "GraphBorderData.h"
#include "GameFramework/Actor.h"
#include "GraphNodeDirection.h"
#include "Components/StaticMeshComponent.h"
#include "GraphNode.generated.h"

class UBoxComponent;
class ANodeManager;
class UGraphBorderData;

UCLASS(Blueprintable)
class SASSC_API AGraphNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGraphNode();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	ANodeManager* GetOrFindNodeManager();
	void SetId(int32 NewId);
	int32 GetId();
	UFUNCTION(BlueprintCallable)
	void SetNodeManager(ANodeManager* NewNodeManager);

	FString ToString() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	ANodeManager* NodeManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory|Debug")
	UStaticMeshComponent* DebugMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	int32 Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory", meta=(ExposeOnSpawn="true"))
	AGraphNode* ParentNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	UGraphBorderData* BorderData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	TArray<AGraphNode*> Connections;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	TArray<int32> IdConnections = {-1, -1, -1, -1, -1, -1, -1, -1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	TArray<bool> HasVisited;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory", meta=(ExposeOnSpawn="true"))
	FVector Normal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	AGraphNode* AStarParentNode;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Territory")
	float AStarFScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	float AStarGScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	bool bIsOpen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	bool bIsClosed;

public:
	float GetAStarFScore() const;
	float GetAStarGScore() const;

	UFUNCTION(BlueprintCallable)
	int32 GetEmpireId() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ColorSelf(FLinearColor NewColor) const;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ColorNeighbors(bool AddColor);
	
	UFUNCTION(BlueprintCallable)
	void SetConnection(AGraphNode* NodeToConnect, EGraphNodeDirection Direction);
	UFUNCTION(BlueprintCallable)
	AGraphNode* GetConnection(EGraphNodeDirection Direction);
	UFUNCTION(BlueprintCallable)
	void SetVisited(int Index, bool HasBeenVisited);
	UFUNCTION(BlueprintCallable)
	bool GetHasVisitedAtIndex(int Index);
	UFUNCTION(BlueprintCallable)
	FVector GetLocation();
	void SetParentNode(AGraphNode* NewParent);
	AGraphNode* GetParentNode() const;
	void SetNormal(FVector NewNormal);
	UFUNCTION(BlueprintCallable)
	FVector GetNormal();
	UFUNCTION(BlueprintCallable)
	void SetAStarStatus(AGraphNode* NewAStarParentNode, float NewAStarFScore, float NewAStarGScore);
	UFUNCTION(BlueprintCallable)
	void ClearBorderData();
	UFUNCTION(BlueprintCallable)
	bool HasValidBorderData();
	UFUNCTION(BlueprintCallable)
	void SetBorder(UGraphBorder* Border);
	UFUNCTION(BlueprintCallable)
	UGraphBorder* GetBorder();
	UFUNCTION(BlueprintCallable)
	void SetBorderNext(AGraphNode* GraphNode);
	UFUNCTION(BlueprintCallable)
	AGraphNode* GetBorderNext();
	UFUNCTION(BlueprintCallable)
	void SetBorderPrev(AGraphNode* GraphNode);
	UFUNCTION(BlueprintCallable)
	AGraphNode* GetBorderPrev();
	UGraphBorderData* GetBorderData() const;
	void ResetBorderData();
	void SetBorderData(UGraphBorderData* const NewBorderData);
	bool IsOpen() const;
	void SetIsOpen(const bool NewIsOpen);
	bool IsClosed() const;
	void SetIsClosed(const bool NewIsClosed);
	UFUNCTION(BlueprintCallable)
	void ResetTraversalFlags();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
