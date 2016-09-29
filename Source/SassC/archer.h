// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "unitBase.h"
#include "archer.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API Aarcher : public AunitBase
{
	GENERATED_BODY()

	Aarcher();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	virtual void Attack_Implementation(AActor* Target) override;
	virtual bool Attack_Validate(AActor* Target) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float ArcherAttackRange = 35.0f;

};
