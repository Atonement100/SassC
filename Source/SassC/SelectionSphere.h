// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SelectionSphere.generated.h"

UCLASS()
class SASSC_API ASelectionSphere : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASelectionSphere();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Selection Sphere")
	UStaticMeshComponent* Sphere;
};
