// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "buildingBase.h"
#include "Gate.generated.h"

/**
 * 
 */

UCLASS()
class SASSC_API AGate : public ABuildingBase
{
	GENERATED_BODY()

public:
	AGate();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/*
		UFUNCTION(Reliable, NetMulticast, WithValidation)
		void ColorBldg(FLinearColor PlayerColor);
		virtual void ColorBldg_Implementation(FLinearColor PlayerColor) override;
		virtual bool ColorBldg_Validate(FLinearColor PlayerColor) override;
		*/
	UFUNCTION()
	virtual void OnOverlapBegin_GateTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                        const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_GateTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
