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
	virtual void UpdateMaterial(FLinearColor PlayerColor);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorBldg(FLinearColor PlayerColor);
	virtual void ColorBldg_Implementation(FLinearColor PlayerColor);
	virtual bool ColorBldg_Validate(FLinearColor PlayerColor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building Base")
	TArray<FVector> CornerLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building Base")
	int32 OwningPlayerID;

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	UFUNCTION()
	float GetHealth();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building Base")
	FVector TraceSize = FVector(50, 50, 10);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building Base")
	FVector HalfHeight = FVector(0, 0, 25);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void PostCreation();
	virtual void PostCreation_Implementation();
	virtual bool PostCreation_Validate();

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
