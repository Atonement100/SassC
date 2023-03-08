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
	virtual ETypeOfEntity GetTypeOfEntity() override {return this->TypeOfBuilding;}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::House;
};
