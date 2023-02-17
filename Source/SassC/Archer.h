// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitBase.h"
#include "Archer.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AArcher : public AUnitBase
{
	GENERATED_BODY()

	AArcher();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void Attack_Implementation(AActor* Target) override;
	virtual bool Attack_Validate(AActor* Target) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackRange123 = 50.0f;
};
