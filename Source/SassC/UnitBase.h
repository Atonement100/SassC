// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "UnitBase.generated.h"

class ABuildingBase;

UENUM()
enum class EProcessingCommandType : uint8 {
	ORDER_IDLE			UMETA(DisplayName = "Idle"),
	ORDER_UNIT			UMETA(DisplayName = "Unit"),
	ORDER_BUILDING		UMETA(DisplayName = "Building"),
	ORDER_WORLD			UMETA(DisplayName = "World"),
	ORDER_STATIC_UNIT	UMETA(DisplayName = "StaticUnit")
};

UCLASS()
class SASSC_API AUnitBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitBase();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnOverlapBegin_DetectionSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_DetectionSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void OnOverlapBegin_AggroSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_AggroSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void SetDecalVisibility(bool isVisible);

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void UpdateMaterial(FLinearColor PlayerColor);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorUnitDecal(FLinearColor PlayerColor);
	virtual void ColorUnitDecal_Implementation(FLinearColor PlayerColor);
	virtual bool ColorUnitDecal_Validate(FLinearColor PlayerColor);

	const FName ColorParameterText = "PlayerColor";

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorUnit(FLinearColor PlayerColor);
	virtual void ColorUnit_Implementation(FLinearColor PlayerColor);
	virtual bool ColorUnit_Validate(FLinearColor PlayerColor);

	/*Move selected units towards a world*/
	UFUNCTION(Reliable, Server, WithValidation)
	void MoveToDest(FVector Destination);
	virtual void MoveToDest_Implementation(FVector Destination);
	virtual bool MoveToDest_Validate(FVector Destination);

	/*Move selected units towards an enemy unit*/
	UFUNCTION(Reliable, Server, WithValidation)
	void MoveToUnit(AActor* UnitToAttack, bool IsStaticAttack = false);
	virtual void MoveToUnit_Implementation(AActor* UnitToAttack, bool IsStaticAttack = false);
	virtual bool MoveToUnit_Validate(AActor* UnitToAttack, bool IsStaticAttack = false);

	/*Move selected units towards an enemy building*/
	UFUNCTION(Reliable, Server, WithValidation)
	void MoveToBuilding(AActor* BuildingToTarget);
	virtual void MoveToBuilding_Implementation(AActor* BuildingToTarget);
	virtual bool MoveToBuilding_Validate(AActor* BuildingToTarget);

	UFUNCTION(Reliable, Server, WithValidation)
	void Attack(AActor* Target);
	virtual void Attack_Implementation(AActor* Target);
	virtual bool Attack_Validate(AActor* Target);

	UFUNCTION(Reliable, Server, WithValidation)
	void StartAttackAnimation();
	virtual void StartAttackAnimation_Implementation();
	virtual bool StartAttackAnimation_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void StopAttackAnimation();
	virtual void StopAttackAnimation_Implementation();
	virtual bool StopAttackAnimation_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void SetIsAttacking(bool NewIsAttacking);
	virtual void SetIsAttacking_Implementation(bool NewIsAttacking);
	virtual bool SetIsAttacking_Validate(bool NewIsAttacking);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	FVector TraceSize = FVector(5, 5, 10);

	UFUNCTION(Reliable, Server, WithValidation)
	void FixSpawnLocation(FVector RealLocation);
	virtual void FixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool FixSpawnLocation_Validate(FVector RealLocation);

protected:
	void SwitchToIdle();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UMaterialInstanceDynamic* UnitDecalMaterialDynamic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UMaterialInstanceDynamic* UnitMeshMaterialDynamic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	FName ColorParameterName = "PlayerColor";
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float Health = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UDecalComponent* UnderUnitDecal;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UDecalComponent* SelectionBlendDecal;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UStaticMeshComponent* DetectionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UStaticMeshComponent* AggroSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	FVector OrderDestination;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	FVector OrderDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	AUnitBase* ActorToFollow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	ABuildingBase* BuildingToAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	bool ReachedBuilding;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float MaxTimeToMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float TimeSinceOrdered = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	float TimeSinceAttack = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackDelay = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackRange = 13.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	float AttackDamage = 15.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	TArray<AActor*> EnemiesInRange;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	bool IsAttacking;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base") //Used for units with projectiles
	UClass* ProjectileClass;

	const int SelectionSphereScaleMod = 100; //this should never ever be changed
	
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void NetFixSpawnLocation(FVector RealLocation);
	virtual void NetFixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool NetFixSpawnLocation_Validate(FVector RealLocation);

	UFUNCTION(Unreliable, NetMulticast, WithValidation)
	void SpawnProjectile(FVector TargetLocation);
	virtual void SpawnProjectile_Implementation(FVector TargetLocation);
	virtual bool SpawnProjectile_Validate(FVector TargetLocation);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Base")
	EProcessingCommandType ActiveCommandType = EProcessingCommandType::ORDER_IDLE;
};
