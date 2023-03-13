// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/BuildingRequirements.h"
#include "Core/EntityInterface.h"
#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "Gamemode/Sassilization/Territory/TerritoryInfo.h"
#include "BuildingBase.generated.h"

class AEmpire;
class UMeshComponent;
class UBoxComponent;
class UStreamableRenderAsset;
class ASassPlayerState;
class UMaterialInstanceDynamic;

UCLASS()
class SASSC_API ABuildingBase : public AActor, public IEntityInterface
{
	GENERATED_BODY()

public:
	ABuildingBase();

	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;

#pragma region IEntityInterface
	virtual ETypeOfEntity GetTypeOfEntity() const override {return ETypeOfEntity::City;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts();}
	virtual FBox GetSpawnBoundingBox() const override 
	{
		FVector Origin, Extent;
		GetActorBounds(true, Origin, Extent, true);
		return FBox(Origin, Extent);
	}
	
	virtual FBuildingRequirements GetRequirementsForLevel(int Level) const override
	{
		if (!GetBuildingRequirements().IsValidIndex(Level))
		{
			return FBuildingRequirements();
		}

		return GetBuildingRequirements()[Level];
	}
	
	virtual void WhenBuilt() override;
	virtual bool IsBuilding() const override {return true;}
	virtual bool IsUnit() const override {return false;}
	virtual void Initialize(AEmpire* NewEmpire) override;
	virtual void SetControl(AEmpire* NewEmpire) override;
	virtual AEmpire* GetEmpire() const override {return this->ControllingEmpire;}
#pragma endregion

	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const { return this->LevelRequirements; }
	
	/*This function only calls ColorBldg. I believe I set it up this way because ColorBldg is a NetMulticast and needs a server-based actor, such as the bldg itself, to properly call NetMulticast events.*/
	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void UpdateMaterial(FLinearColor PlayerColor, bool SetPersistentColor = false);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorBuilding(FLinearColor PlayerColor, bool SetPersistentColor = false);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building Base")
	TArray<FVector> CornerLocations;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Building Base")
	int32 OwningPlayerID;

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, class AActor* DamageCauser) override;

	UFUNCTION()
	float GetHealth();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building Base")
	FVector TraceSize = FVector(35.0f, 31.0f, 10.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building Base")
	FVector HalfHeight = FVector(0.0f, 0.0f, 20.0f);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void PostCreation(FLinearColor PlayerColor);

	UFUNCTION(Reliable, Server, WithValidation)
	void FixSpawnLocation(FVector RealLocation);

#pragma region Upgradable Building Functions
	virtual void HideMesh();
	virtual void ShowMesh();
	virtual UMeshComponent* GetMesh(int MeshIndex = 0);

	//todo I don't see why this would have needed to be an RPC
	// UFUNCTION(Unreliable, Client)
	virtual void PreviewUpgrade();
	// virtual void PreviewUpgrade_Implementation();

	// UFUNCTION(Unreliable, Client)
	virtual void ResetPreview();
	// virtual void ResetPreview_Implementation();

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetUpgradeBuilding();
	virtual void NetUpgradeBuilding_Implementation();
	virtual bool NetUpgradeBuilding_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void UpgradeBuilding();
	virtual void UpgradeBuilding_Implementation();
	virtual bool UpgradeBuilding_Validate();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool PreviewActive = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool ResetRequired = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool DelayReset = false;
#pragma endregion

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	TArray<FBuildingRequirements> LevelRequirements = {
		{ FBuildingRequirements( { {ETypeOfEntity::City, FBuildingRequirement(1, 0)} }) }
	};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UMeshComponent* ActiveBuildingMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	TArray<UStreamableRenderAsset*> AvailableBuildingMeshes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UBoxComponent* BuildingCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	TArray<UMaterialInstanceDynamic*> BldgMeshMaterialDynamic = TArray<UMaterialInstanceDynamic*>();
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float Health = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	TArray<float> InitialHealths;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	TArray<float> AttackSpeeds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	TArray<float> AttackRanges;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	TArray<float> AttackDamages;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	TArray<FResourceCosts> ResourceCosts;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	float Influence = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	float BuildTime = 50.0f;

	/*
	 * Base value of a building to be used when the building itself is destroyed.
	 * This value is detracted from the player losing the building and awarded to
	 * the player who destroyed it. Additional gold is awarded to the attacker,
	 * of the sum GoldBonusOnDestroy.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	uint8 GoldValueOnDestroy = 10;
	
	/*
	 * See also GoldValueOnDestroy. This sum is awarded to empire who destroyed
	 * this building.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Base")
	uint8 GoldBonusOnDestroy = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	FTerritoryInfo TerritoryInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool bIsDestroyed = false;

public:
	FTerritoryInfo GetTerritoryInfo() const;
	void SetTerritoryInfo(const FTerritoryInfo& NewTerritoryInfo);
	bool IsDestroyed() const;
	void SetIsDestroyed(const bool bNewIsDestroyed);
	virtual float GetInfluence() {return this->Influence;}
	bool IsBuilt() const {return this->bIsBuilt;}
	uint8 GetUpgradeLevel() const {return this->UpgradeLevel;}
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool CanPlayerSpawn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool CanSpawnFoundation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	ASassPlayerState* OwningPlayerState;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	FName ColorParameterText = "PlayerColor";
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector CollisionBounds = FVector(35.0f, 31.0f, 40.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector CollisionDisplacement = FVector(0.0f, 1.0f, 20.0f);
	
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Empire")
	AEmpire* ControllingEmpire;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	FLinearColor OwningPlayerColor;

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetFixSpawnLocation(FVector RealLocation);
	virtual void NetFixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool NetFixSpawnLocation_Validate(FVector RealLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool bIsBuilt = false;
	//These properties are used for buildings with upgrades, i.e. workshop, tower, and wall->gate. 
	//For any buildings using these properties, each mesh must be individually declared in AvailableBuildingMeshes.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	uint8 UpgradeLevel = 0;
};
