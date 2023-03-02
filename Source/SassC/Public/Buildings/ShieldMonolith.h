// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "BuildingBase.h"
#include "ShieldMonolith.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API AShieldMonolith : public ABuildingBase
{
	GENERATED_BODY()
public:
	AShieldMonolith();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;

	void PostCreation_Implementation(FLinearColor PlayerColor) override;

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void SpawnBeamEmitter(UParticleSystem* PSysToSpawn, AShieldMonolith* SourceMonolith,
	                      AShieldMonolith* TargetMonolith);
	virtual void SpawnBeamEmitter_Implementation(UParticleSystem* PSysToSpawn, AShieldMonolith* SourceMonolith,
	                                             AShieldMonolith* TargetMonolith);
	virtual bool SpawnBeamEmitter_Validate(UParticleSystem* PSysToSpawn, AShieldMonolith* SourceMonolith,
	                                       AShieldMonolith* TargetMonolith);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void DestroyConnectedBeamEmitters();
	virtual void DestroyConnectedBeamEmitters_Implementation();
	virtual bool DestroyConnectedBeamEmitters_Validate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield Monolith")
	UParticleSystem* BeamPSys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Monolith")
	TArray<UParticleSystemComponent*> AttachedParticleSystems;
	USceneComponent* GetBeamSocket();
	TArray<AShieldMonolith*> FindMonolithsInRange(float Range = 80.0f,
	                                              TArray<AActor*> ActorsToIgnore = TArray<AActor*>()) const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Monolith")
	UParticleSystem* BubblePSys;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield Monolith")
	FVector BubblePSysLocation = FVector(0, 0, 20);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Monolith")
	USceneComponent* BeamPSysSocket;
};