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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
		UMaterialInstanceDynamic* UnitDecalMaterialDynamic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
		UMaterialInstanceDynamic* UnitMeshMaterialDynamic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
		FName ColorParameterName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
		float Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	USkeletalMeshComponent* UnitMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UDecalComponent* UnderUnitDecal;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UDecalComponent* SelectionCircleDecal;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UStaticMeshComponent* DetectionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UTextRenderComponent* TextRender;
	
};
