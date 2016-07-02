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
	virtual void OnOverlapBegin_DetectionSphere(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_DetectionSphere(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void OnOverlapBegin_AggroSphere(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_AggroSphere(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void SetDecalVisibility(bool isVisible);

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void UpdateMaterial(FLinearColor PlayerColor);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorUnitDecal(FLinearColor PlayerColor);
	virtual void ColorUnitDecal_Implementation(FLinearColor PlayerColor);
	virtual bool ColorUnitDecal_Validate(FLinearColor PlayerColor);

	/*Move selected units towards a world or enemy building destination*/
	UFUNCTION(Reliable, Server, WithValidation)
	void MoveToDest(FVector Destination);
	virtual void MoveToDest_Implementation(FVector Destination);
	virtual bool MoveToDest_Validate(FVector Destination);

	/*Move selected units towards an enemy unit*/
	UFUNCTION(Reliable, Server, WithValidation)
	void MoveToUnit(AActor* UnitToAttack);
	virtual void MoveToUnit_Implementation(AActor* UnitToAttack);
	virtual bool MoveToUnit_Validate(AActor* UnitToAttack);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UDecalComponent* SelectionCircleDecal;

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Unit Base")
	int32 OwningPlayerID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UTextRenderComponent* TextRender;

	UFUNCTION()
	float GetHealth();

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
	UStaticMeshComponent* AggroSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	FVector OrderDestination;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	FVector OrderDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	AActor* ActorToFollow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	bool ProcessingMoveToWorldOrder;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	bool ProcessingMoveToUnitOrder;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float MaxTimeToMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float TimeSinceOrdered = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float TimeSinceAttack = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackDelay = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackRange = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackDamage = 15.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	TArray<AunitBase*> EnemiesInRange;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	bool IsAttacking;


	const FName NoAggroTag = "NULLIFY_OVERLAP"; //this should never ever be changed unless also changed in sassplayer
	const int SelectionSphereScaleMod = 100; //this should never ever be changed
	
};
