// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "GameFramework/Actor.h"
#include "GraphNode.generated.h"

UCLASS()
class SASSC_API AGraphNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGraphNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	int Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	AGraphNode* ParentNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	TArray<AGraphNode*> Connections;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
	TArray<bool> HasVisited;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
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
	uint8 GetEmpireId() const;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
