// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "unitBase.generated.h"

UCLASS()
class SASSC_API AunitBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AunitBase();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION()
	virtual void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void SetDecalVisibility(UDecalComponent* DecalRef, bool isVisible);

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void UpdateMaterial(FLinearColor PlayerColor);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorUnitDecal(FLinearColor PlayerColor);
	virtual void ColorUnitDecal_Implementation(FLinearColor PlayerColor);
	virtual bool ColorUnitDecal_Validate(FLinearColor PlayerColor);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
		UDecalComponent* SelectionCircleDecal;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UMaterialInstanceDynamic* UnitDecalMaterialDynamic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UMaterialInstanceDynamic* UnitMeshMaterialDynamic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	FName ColorParameterName = "PlayerColor";
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float Health = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	USkeletalMeshComponent* UnitMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UDecalComponent* UnderUnitDecal;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UStaticMeshComponent* DetectionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UTextRenderComponent* TextRender;
	
};
