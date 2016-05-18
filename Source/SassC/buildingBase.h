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
	
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void UpdateMaterial(FLinearColor PlayerColor, AActor* BuildingRef);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorBldg(FLinearColor PlayerColor, AActor* BuildingRef);
	virtual void ColorBldg_Implementation(FLinearColor PlayerColor, AActor* BuildingRef);
	virtual bool ColorBldg_Validate(FLinearColor PlayerColor, AActor* BuildingRef);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UStaticMeshComponent* BuildingMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building Base")
	UBoxComponent* BuildingCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	float BuildingHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	UMaterialInstanceDynamic* BldgMeshMaterialDynamic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Base")
	AsassPlayerState* OwningPlayerState;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	FName ColorParameterText = "PlayerColor";
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector CollisionBounds = FVector(50.0f, 50.0f, 50.0f);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector CollisionDisplacement = FVector(0.0f, 0.0f, 50.0f);

};
