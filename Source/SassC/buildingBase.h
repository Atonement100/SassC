// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "sassPlayerState.h"
#include "buildingBase.generated.h"

UCLASS()
class SASSC_API AbuildingBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AbuildingBase();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick( float DeltaSeconds ) override;

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
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

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
	UFUNCTION(Unreliable, Client)
	void PreviewUpgrade();
	virtual void PreviewUpgrade_Implementation();

	UFUNCTION(Unreliable, Client)
	void ResetPreview();
	virtual void ResetPreview_Implementation();

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
#pragma endregion

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UStaticMeshComponent* BuildingMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UBoxComponent* BuildingCollision;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float Health = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	TArray<UMaterialInstanceDynamic*> BldgMeshMaterialDynamic = TArray<UMaterialInstanceDynamic*>();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	AsassPlayerState* OwningPlayerState;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	FName ColorParameterText = "PlayerColor";
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector CollisionBounds = FVector(35.0f, 31.0f, 40.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector CollisionDisplacement = FVector(0.0f, 1.0f, 20.0f);
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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	bool ResetRequired = false;

	
};
