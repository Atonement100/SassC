// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "BuildingBase.h"
#include "House.generated.h"

UCLASS()
class SASSC_API AHouse : public ABuildingBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHouse();
	virtual ETypeOfEntity GetTypeOfEntity() const override {return this->TypeOfBuilding;}
	virtual float GetInfluence() override {return this->Influence;}
	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const override { return this->LevelRequirements; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::House;
	float Influence = 128.5875f;
	TArray<FBuildingRequirements> LevelRequirements = {};
};
