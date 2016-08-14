// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "gate.generated.h"

/**
 * 
 */

UCLASS()
class SASSC_API Agate : public AbuildingBase
{
	GENERATED_BODY()
		
public:
	Agate();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorBldg(FLinearColor PlayerColor, int8 MeshLevel = 0);
	virtual void ColorBldg_Implementation(FLinearColor PlayerColor, int8 MeshLevel = 0) override;
	virtual bool ColorBldg_Validate(FLinearColor PlayerColor, int8 MeshLevel = 0) override;

	UFUNCTION()
	virtual void OnOverlapBegin_GateTrigger(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_GateTrigger(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gate")
	bool ShouldBeOpen;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gate")
	USkeletalMeshComponent* GateMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gate")
	UBoxComponent* OpenTrigger;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gate")
	int32 UnitsInRange;
};
