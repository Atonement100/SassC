// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Core/BuildingRequirements.h"
#include "Core/BuildingType.h"
#include "GameFramework/Actor.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Components/MeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StreamableRenderAsset.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BuildingBase.generated.h"

UCLASS()
class SASSC_API ABuildingBase : public AActor
{
	GENERATED_BODY()

public:
	ABuildingBase();

	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;

	/*This function only calls ColorBldg. I believe I set it up this way because ColorBldg is a NetMulticast and needs a server-based actor, such as the bldg itself, to properly call NetMulticast events.*/
	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void UpdateMaterial(FLinearColor PlayerColor, bool SetPersistentColor = false);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorBldg(FLinearColor PlayerColor, bool SetPersistentColor = false);
	virtual void ColorBldg_Implementation(FLinearColor PlayerColor, bool SetPersistentColor = false);
	virtual bool ColorBldg_Validate(FLinearColor PlayerColor, bool SetPersistentColor = false);

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
	virtual void PostCreation_Implementation(FLinearColor PlayerColor);
	virtual bool PostCreation_Validate(FLinearColor PlayerColor);

	UFUNCTION(Reliable, Server, WithValidation)
	void FixSpawnLocation(FVector RealLocation);
	virtual void FixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool FixSpawnLocation_Validate(FVector RealLocation);

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
	TArray<FBuildingRequirements> LevelRequirements = TArray(std::initializer_list<FBuildingRequirements>(
		{FBuildingRequirements(EBuildingType::City, FBuildingRequirement(1, 0))}));
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float IronCost = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float FoodCost = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float GoldCost = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float Influence = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float BuildTime = 50.0f;

	/*
	 * Base value of a building to be used when the building itself is destroyed.
	 * This value is detracted from the player losing the building and awarded to
	 * the player who destroyed it. Additional gold is awarded to the attacker,
	 * of the sum GoldBonusOnDestroy.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	uint8 GoldValueOnDestroy = 10;
	
	/*
	 * See also GoldValueOnDestroy. This sum is awarded to empire who destroyed
	 * this building.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	uint8 GoldBonusOnDestroy = 8;
	
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

	
	UPROPERTY(VisibleAnywhere, Category = "Unit Base")
	UEmpire* ControllingEmpire;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	FLinearColor OwningPlayerColor;

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetFixSpawnLocation(FVector RealLocation);
	virtual void NetFixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool NetFixSpawnLocation_Validate(FVector RealLocation);

	//These properties are used for buildings with upgrades, i.e. workshop, tower, and wall->gate. 
	//For any buildings using these properties, each mesh must be individually declared in the specific blueprint.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	uint8 UpgradeLevel = 0;
};
