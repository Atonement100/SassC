// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "GraphNode.h"
#include "GameFramework/Actor.h"
#include "NodeManager.generated.h"

UCLASS()
class SASSC_API ANodeManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANodeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AGraphNode* FindNearestNode(const FVector Location, const float Radius = 50.0f) const;
};
