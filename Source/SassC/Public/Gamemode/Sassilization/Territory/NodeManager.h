// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "GameFramework/Actor.h"
#include "NodeManager.generated.h"

class AGraphNode;
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
	AGraphNode* FindNearestNode(const FVector Location, const float Radius = 50.0f) const;
	UFUNCTION(BlueprintCallable)
	AGraphNode* GetNodeById(const int32 NodeId);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FloodTerritory(const TArray<FTerritoryInfo>& TerritoryOrigins, TArray<FEmpireBorderData>& TerritoryBordersResult);
};
