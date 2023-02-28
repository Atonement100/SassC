// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GraphBorder.h"
#include "GameFramework/Actor.h"
#include "GraphNodeDirection.h"
#include "Components/StaticMeshComponent.h"
#include "GraphNode.generated.h"

class UGraphBorderData;

UCLASS(Blueprintable)
class SASSC_API AGraphNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGraphNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory|Debug")
	UStaticMeshComponent* DebugMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	int Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory", meta=(ExposeOnSpawn="true"))
	AGraphNode* ParentNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	UGraphBorderData* BorderData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	TArray<AGraphNode*> Connections;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	TArray<bool> HasVisited;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory", meta=(ExposeOnSpawn="true"))
	FVector Normal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	AGraphNode* AStarParentNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	float AStarFScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	float AStarGScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	bool bIsOpen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	bool bIsClosed;

public:
	float GetAStarFScore() const;

	UFUNCTION(BlueprintCallable)
	uint8 GetEmpireId() const;

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
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
