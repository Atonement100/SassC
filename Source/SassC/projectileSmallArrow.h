// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "projectileSmallArrow.generated.h"

UCLASS()
class SASSC_API AprojectileSmallArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AprojectileSmallArrow();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectiles")
		UProjectileMovementComponent* MovementComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectiles")
	USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectiles")
	UStaticMeshComponent* ArrowMesh;

};
