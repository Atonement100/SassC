// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "BuildingBase.h"
#include "Engine/StaticMesh.h"
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
	virtual bool CanPlayerSpawn() const override {return false;}
	virtual FBox GetSpawnBoundingBox(int ForLevel) const override 
	{
		UE_LOG(Sassilization, Display, TEXT("array length %d"), AvailableBuildingMeshes.Num())
		// TODO: This wasn't working when called from EntityManager for house spawns. Not sure why.
		// if (AvailableBuildingMeshes.IsValidIndex(ForLevel))
		// {
		// 	UE_LOG(Sassilization, Warning, TEXT("Level %d out of bounds for building."), ForLevel)
		// 	ForLevel = 0;
		// }

		const FBox RealBounds = Cast<UStaticMesh>(this->AvailableBuildingMeshes[ForLevel])->GetBoundingBox();
		
		return FBox(FVector::Zero(), RealBounds.Max - RealBounds.Min);
	}
	virtual void WhenBuilt() override;
	
	void SetHouseType(const int NewHouseType)
	{
		// TODO?: Update this->BuildingCollision or just rely on the mesh itself?
		this->HouseType = NewHouseType;

		if (AvailableBuildingMeshes.IsValidIndex(NewHouseType))
		{
			Cast<UStaticMeshComponent>(this->ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[NewHouseType]));
		}

		if (NewHouseType == 1) // TODO Need to figure out a better way for this. Maybe worth the effort to rework CanFit to use a proper bounding box instead of a Zero-To-Max box.
		{
			this->ActiveBuildingMesh->SetRelativeLocation(FVector(0, 10, 0));
		}
		else
		{
			this->ActiveBuildingMesh->SetRelativeLocation(FVector(0, 0, 0));
		}
	}
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	ETypeOfEntity TypeOfBuilding = ETypeOfEntity::House;
	float Influence = 128.5875f;
	TArray<FBuildingRequirements> LevelRequirements = {};
	int HouseType = 0;
};
