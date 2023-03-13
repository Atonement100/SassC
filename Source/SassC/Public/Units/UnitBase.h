// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gamemode/Sassilization/Empire.h"
#include "GameFramework/Character.h"
#include "Core/SassCStaticLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/EntityInterface.h"
#include "UnitBase.generated.h"

class ABuildingBase;
class UTextRenderComponent;

UENUM()
enum class EProcessingCommandType : uint8
{
	ORDER_IDLE UMETA(DisplayName = "Idle"),
	ORDER_UNIT UMETA(DisplayName = "Unit"),
	ORDER_BUILDING UMETA(DisplayName = "Building"),
	ORDER_WORLD UMETA(DisplayName = "World"),
	ORDER_STATIC_UNIT UMETA(DisplayName = "StaticUnit")
};

USTRUCT()
struct FUnitCombatProperties
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Unit Base")
	float AttackDelay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Unit Base")
	float AttackRange;
	
	UPROPERTY(EditDefaultsOnly, Category = "Unit Base")
	float AttackDamage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Unit Base")
	float AggroRange;

	/** Defaults **/
	FUnitCombatProperties()
	{
		AttackDelay = 2.0f;
		AttackRange = 13.0f;
		AttackDamage = 15.0f;
		AggroRange = AttackRange / USassCStaticLibrary::AttackRangeToAggroRangeModifier();
	}
	
	FString ToString() const
	{
		return "AttackDelay: " + FString::SanitizeFloat(this->AttackDelay)
			+ " AttackRange: " + FString::SanitizeFloat(this->AttackRange)
			+ " AttackDamage: " + FString::SanitizeFloat(this->AttackDamage)
			+ " AggroRange: " + FString::SanitizeFloat(this->AggroRange);
	}
};

UCLASS()
class SASSC_API AUnitBase : public ACharacter, public IEntityInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* SetupInputComponent) override;
	virtual void PostInitializeComponents() override;

#pragma region IEntityInterface
	virtual ETypeOfEntity GetTypeOfEntity() const override {return ETypeOfEntity::Peasant;}
	virtual FResourceCosts GetResourceCosts() const override {return FResourceCosts();}
	virtual FBox GetSpawnBoundingBox() const override 
	{
		return this->GetCapsuleComponent()->GetLocalBounds().GetBox();
	}
	
	virtual FBuildingRequirements GetRequirementsForLevel(int Level) const override
	{
		if (!GetBuildingRequirements().IsValidIndex(Level))
		{
			return FBuildingRequirements();
		}

		return GetBuildingRequirements()[Level];
	}
	
	virtual bool IsBuilding() const override {return false;}
	virtual bool IsUnit() const override {return true;}
	virtual void Initialize(AEmpire* NewEmpire) override;
	virtual void SetControl(AEmpire* NewEmpire) override;
	virtual AEmpire* GetEmpire() const override {return this->ControllingEmpire;}
	virtual FVector GetSpawnOffset() override {return FVector(0, 0, this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());}
#pragma endregion

	virtual TArray<FBuildingRequirements> GetBuildingRequirements() const { return {FBuildingRequirements()};};

	UFUNCTION()
	virtual void OnOverlapBegin_DetectionSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                            const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_DetectionSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void OnOverlapBegin_AggroSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                        const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd_AggroSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Spawnables Functions")
	virtual void SetDecalVisibility(bool bIsVisible);

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
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, class AActor* DamageCauser) override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Unit Base")
	int32 OwningPlayerID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	UTextRenderComponent* TextRender;

	UFUNCTION()
	float GetHealth();

	UFUNCTION()
	virtual UClass* GetProjectileClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base")
	FVector TraceSize = FVector(5, 5, 10);

	UFUNCTION(Reliable, Server, WithValidation)
	void FixSpawnLocation(FVector RealLocation);
	virtual void FixSpawnLocation_Implementation(FVector RealLocation);
	virtual bool FixSpawnLocation_Validate(FVector RealLocation);

	UFUNCTION(BlueprintCallable, Category = "Empire|Unit")
	bool IsUnitSelected() const;
	UFUNCTION(BlueprintCallable, Category = "Empire|Unit")
	void SelectUnit(bool bShouldSelect);

protected:
	void SwitchToIdle();

	UPROPERTY(VisibleAnywhere, Category = "Unit Base")
	AEmpire* ControllingEmpire;
	UPROPERTY(EditDefaultsOnly, Category = "Unit Base")
	FUnitCombatProperties UnitCombatProperties;
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	TArray<AActor*> EnemiesInRange;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Base")
	bool bIsUnitSelected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit Base") //Used for units with projectiles
	UClass* ProjectileClass;

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
