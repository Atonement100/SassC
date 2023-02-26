// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/UnitBase.h"
#include "Buildings/BuildingBase.h"
#include "Player/SassPlayer.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "AI/UnitController.h"
#include "Effects/ProjectileSmallArrow.h"
#include "Net/UnrealNetwork.h"
#include "Core/SassCStaticLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DamageEvents.h"
#include "Gamemode/Sassilization/Empire.h"

AUnitBase::AUnitBase()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 30.0f;

	UnitCombatProperties = Cast<AUnitBase>(this->GetClass()->GetDefaultObject())->UnitCombatProperties;
	UnitCombatProperties.AggroRange = UnitCombatProperties.AttackRange / USassCStaticLibrary::AttackRangeToAggroRangeModifier();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, UnitCombatProperties.ToString());
	}
	
	UnderUnitDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Under Unit Decal"));
	UnderUnitDecal->SetupAttachment(RootComponent);
	UnderUnitDecal->SetFadeScreenSize(0.0f);
	UnderUnitDecal->SetRelativeLocationAndRotation(FVector(0, 0, -10.2f), FQuat(FRotator(-90.0f, 0, 0)));
	UnderUnitDecal->SetRelativeScale3D(FVector(10, 5, 5));
	UnderUnitDecal->DecalSize = FVector(1, 1, 1);

	SelectionCircleDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Circle Decal"));
	SelectionCircleDecal->SetupAttachment(RootComponent);
	SelectionCircleDecal->SetVisibleFlag(false);
	SelectionCircleDecal->SetFadeScreenSize(0.0f);
	SelectionCircleDecal->SetRelativeLocationAndRotation(FVector(0, 0, -9), FQuat(FRotator(-90.0f, 0, 0)));
	SelectionCircleDecal->SetRelativeScale3D(FVector(10, 5, 5));
	SelectionCircleDecal->DecalSize = FVector(1, 1, 1);

	DetectionSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Detection Sphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetRelativeLocation(FVector(0, 0, -9.0f));
	DetectionSphere->SetRelativeScale3D(FVector(.32, .32, .2));
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AUnitBase::OnOverlapBegin_DetectionSphere);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AUnitBase::OnOverlapEnd_DetectionSphere);

	AggroSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Aggro Sphere"));
	AggroSphere->ComponentTags.Add(USassCStaticLibrary::NoAggroTag());
	AggroSphere->SetupAttachment(RootComponent);
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AUnitBase::OnOverlapBegin_AggroSphere);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AUnitBase::OnOverlapEnd_AggroSphere);
	//Decide if I want to have smaller aggro radius than attack range for idle characters... This would cause some issues with aggroing 
	//In situations where someone on the offensive sends units /near/ and enemy but does not click him. Maybe lower aggro radius after
	//A period of idle time to offset this?
	FVector TransformToUse = FVector(UnitCombatProperties.AggroRange);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, UKismetStringLibrary::Conv_VectorToString(TransformToUse));

	AggroSphere->SetRelativeScale3D(TransformToUse);
	//TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));
	//TextRender->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	CharMoveComp->SetCrouchedHalfHeight(5.0f);
	CharMoveComp->bOrientRotationToMovement = true;
	CharMoveComp->DefaultWaterMovementMode = EMovementMode::MOVE_Walking;
	CharMoveComp->MaxStepHeight = 0;
	CharMoveComp->MaxWalkSpeed = 50.0f;
	CharMoveComp->MaxWalkSpeedCrouched = 50.0f;
}

void AUnitBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnitBase, OwningPlayerID);
	DOREPLIFETIME(AUnitBase, bIsAttacking);
	DOREPLIFETIME(AUnitBase, Health);
}

void AUnitBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (GetMesh()) { UnitMeshMaterialDynamic = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0)); }
}


void AUnitBase::OnOverlapBegin_DetectionSphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                                               class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                               bool bFromSweep, const FHitResult& SweepResult)
{
	ASassPlayer* PlayerCharacterRef = (ASassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder())
	{
		ASassPlayerState* PlayerStateRef = PlayerCharacterRef->GetPlayerState<ASassPlayerState>();
		if (PlayerStateRef != nullptr && this->OwningPlayerID == PlayerStateRef->GetPlayerId())
		{
			SetDecalVisibility(true);
		}
	}
}

void AUnitBase::OnOverlapEnd_DetectionSphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                                             class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASassPlayer* PlayerCharacterRef = (ASassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder())
	{
		ASassPlayerState* PlayerStateRef = PlayerCharacterRef->GetPlayerState<ASassPlayerState>();
		if (PlayerStateRef != nullptr && this->OwningPlayerID == PlayerStateRef->GetPlayerId())
		{
			SetDecalVisibility(false);
		}
	}
}

void AUnitBase::OnOverlapBegin_AggroSphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                                           class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult)
{
	if (OtherActor != this && !OtherComp->ComponentHasTag(USassCStaticLibrary::NoAggroTag()))
	{
		if (OtherActor->IsA(AUnitBase::StaticClass()) && Cast<AUnitBase>(OtherActor)->OwningPlayerID != OwningPlayerID)
		{
			EnemiesInRange.Add(OtherActor);
		}
		else if (OtherActor->IsA(ABuildingBase::StaticClass()) && Cast<ABuildingBase>(OtherActor)->OwningPlayerID !=
			OwningPlayerID)
		{
			EnemiesInRange.Add(OtherActor);
			if (OtherActor == BuildingToAttack) { ReachedBuilding = true; }
		}
	}
}

void AUnitBase::OnOverlapEnd_AggroSphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                                         class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;
	EnemiesInRange.Remove(OtherActor);
	if (OtherActor == BuildingToAttack) { ReachedBuilding = false; }
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
}

void AUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeSinceAttack += DeltaTime;

	switch (ActiveCommandType)
	{
	case EProcessingCommandType::ORDER_IDLE:
		if (EnemiesInRange.Num() > 0 && TimeSinceAttack > UnitCombatProperties.AttackDelay && EnemiesInRange[0])
		{
			if (AUnitBase* Unit = Cast<AUnitBase>(EnemiesInRange[0]))
			{
				if (Unit->OwningPlayerID != this->OwningPlayerID)
				{
					MoveToUnit(Unit, true);
				}
				else
				{
					EnemiesInRange.RemoveAt(0);
				}
			}
			else if (ABuildingBase* Bldg = Cast<ABuildingBase>(EnemiesInRange[0]))
			{
				if (Bldg->OwningPlayerID != this->OwningPlayerID)
				{
					MoveToBuilding(Bldg);
				}
				else
				{
					EnemiesInRange.RemoveAt(0);
				}
			}
		}
		else if (bIsAttacking && EnemiesInRange.Num() == 0)
		{
			SetIsAttacking(false);
		}
		break;
	case EProcessingCommandType::ORDER_STATIC_UNIT:
		if (ActorToFollow)
		{
			OrderDirection = ActorToFollow->GetActorLocation() - GetActorLocation();
			if (OrderDirection.Size() < UnitCombatProperties.AttackRange)
			{
				SetActorRotation(FRotator(0, OrderDirection.Rotation().Yaw, 0));
				if (TimeSinceAttack > UnitCombatProperties.AttackDelay)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Static Attack!!");
					TimeSinceAttack = 0.0f;
					if (ActorToFollow->GetHealth() > 0)
					{
						Attack(ActorToFollow);
						if (!bIsAttacking) SetIsAttacking(true);
					}
					else
					{
						ActorToFollow = nullptr;
						SwitchToIdle();
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver,
						                                 "Static Attack target has been killed");
					}
				}
			}
			else
			{
				ActorToFollow = nullptr;
				SwitchToIdle();
			}
		}
		else
		{
			SwitchToIdle();
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete");
		}

		break;
	case EProcessingCommandType::ORDER_WORLD:
		AddMovementInput(OrderDirection, 1.0f);
		TimeSinceOrdered += DeltaTime;
		if ((OrderDestination - GetActorLocation()).Size2D() < 5.0f || TimeSinceOrdered > MaxTimeToMove)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "UnitBase Movement Complete");
			SwitchToIdle();
		}

		break;
	case EProcessingCommandType::ORDER_UNIT:
		if (ActorToFollow)
		{
			OrderDirection = ActorToFollow->GetActorLocation() - GetActorLocation();
			if (OrderDirection.Size() > UnitCombatProperties.AttackRange) AddMovementInput(OrderDirection, 1.0f);
			SetActorRotation(FRotator(0, OrderDirection.Rotation().Yaw, 0));

			if (OrderDirection.Size() < UnitCombatProperties.AttackRange && TimeSinceAttack > UnitCombatProperties.AttackDelay)
			{
				//No need to check if unit is friendly or hostile for this attack, as "ActorToFollow" can only ever be hostile.
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Attack!!");
				TimeSinceAttack = 0.0f;
				if (ActorToFollow->GetHealth() > 0)
				{
					Attack(ActorToFollow);
					if (!bIsAttacking) SetIsAttacking(true);
				}
				else
				{
					ActorToFollow = nullptr;
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver,
					                                 "UnitBase Unit Attack Chase Complete (target has been killed)");
					SwitchToIdle();
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete");
			SwitchToIdle();
		}

		break;
	case EProcessingCommandType::ORDER_BUILDING:

		if (BuildingToAttack && BuildingToAttack->GetHealth() > 0)
		{
			if (!ReachedBuilding)
			{
				OrderDirection = BuildingToAttack->GetActorLocation() - this->GetActorLocation();
				AddMovementInput(OrderDirection, 1.0f);
				if (bIsAttacking) SetIsAttacking(false);

				TimeSinceOrdered += DeltaTime;
				if (TimeSinceOrdered > MaxTimeToMove)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
					                                 "Unitbase could not move to building in time");
					SwitchToIdle();
				}
			}
			else if (TimeSinceAttack > UnitCombatProperties.AttackDelay)
			{
				if (BuildingToAttack->GetHealth() > 0)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "UnitBase: City attack");
					Attack(BuildingToAttack);
					if (!bIsAttacking) SetIsAttacking(true);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange,
					                                 "UnitBase Building Attack Complete (target has been killed)");
					BuildingToAttack = nullptr;
					SwitchToIdle();
				}
			}
		}
		else
		{
			SwitchToIdle();
		}

		break;
	default:
		SwitchToIdle();
		break;
	}
}

void AUnitBase::SwitchToIdle()
{
	ActiveCommandType = EProcessingCommandType::ORDER_IDLE;
	if (bIsAttacking) SetIsAttacking(false);
}

void AUnitBase::Attack_Implementation(AActor* Target)
{
	this->TimeSinceAttack = 0.0f;

	if (!Target)
	{
		SetIsAttacking(false);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
		                                 "UnitBase Attack: Attack failed, target does not exist");
		return;
	}

	const FDamageEvent DamageInfo = FDamageEvent();
	Target->TakeDamage(UnitCombatProperties.AttackDamage, DamageInfo, nullptr, this);
	this->SetActorRotation(FRotator(
		0, UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Target->GetActorLocation()).Yaw, 0));

	if (this->GetProjectileClass())
	{
		SpawnProjectile(Target->GetActorLocation());
	}
}

bool AUnitBase::Attack_Validate(AActor* Target)
{
	return true;
}

void AUnitBase::SpawnProjectile_Implementation(FVector TargetLocation)
{
	//This and related functions are not expressed in unitBase because each projectile-shooting 
	//unit has the potential for very unreleated projectile logic.
	FVector TargetDisplacement = ((TargetLocation + FVector(
		0, 0, this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())) - this->GetActorLocation());

	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	const FVector Location = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket"));
	const FRotator Rotation = TargetDisplacement.Rotation();

	AActor* Projectile = GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation, SpawnParams);
	FVector Velocity = Cast<AProjectileSmallArrow>(Projectile)->MovementComponent->Velocity;
	if (Projectile) Projectile->SetLifeSpan((TargetDisplacement).Size() / Velocity.Size());
}

bool AUnitBase::SpawnProjectile_Validate(FVector TargetLocation)
{
	return true;
}


void AUnitBase::StartAttackAnimation_Implementation()
{
	this->bIsAttacking = true;
}

bool AUnitBase::StartAttackAnimation_Validate()
{
	return true;
}

void AUnitBase::StopAttackAnimation_Implementation()
{
	this->bIsAttacking = false;
}

bool AUnitBase::StopAttackAnimation_Validate()
{
	return true;
}

void AUnitBase::SetIsAttacking_Implementation(bool NewIsAttacking)
{
	this->bIsAttacking = NewIsAttacking;
}

bool AUnitBase::SetIsAttacking_Validate(bool NewIsAttacking)
{
	return true;
}

void AUnitBase::SetupPlayerInputComponent(class UInputComponent* SetupInputComponent)
{
	Super::SetupPlayerInputComponent(SetupInputComponent);
}

void AUnitBase::MoveToDest_Implementation(FVector Destination)
{
	if (bIsAttacking)SetIsAttacking(false);
	OrderDestination = Destination;
	OrderDirection = Destination - GetActorLocation();
	TimeSinceOrdered = 0;
	MaxTimeToMove = OrderDirection.Size() / GetMovementComponent()->GetMaxSpeed();
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase MaxTimeToMove = " + FString::SanitizeFloat(MaxTimeToMove));
	ActiveCommandType = EProcessingCommandType::ORDER_WORLD;
}

bool AUnitBase::MoveToDest_Validate(FVector Destination)
{
	return true;
}

void AUnitBase::MoveToUnit_Implementation(AActor* UnitToAttack, bool IsStaticAttack)
{
	if (bIsAttacking)SetIsAttacking(false);
	ActorToFollow = Cast<AUnitBase>(UnitToAttack);
	if (IsStaticAttack)
	{
		ActiveCommandType = EProcessingCommandType::ORDER_STATIC_UNIT;
	}
	else
	{
		ActiveCommandType = EProcessingCommandType::ORDER_UNIT;
	}
}

bool AUnitBase::MoveToUnit_Validate(AActor* UnitToAttack, bool IsStaticAttack)
{
	return true;
}

void AUnitBase::MoveToBuilding_Implementation(AActor* BuildingToTarget)
{
	if (bIsAttacking)SetIsAttacking(false);
	OrderDirection = BuildingToTarget->GetActorLocation() - GetActorLocation();
	TimeSinceOrdered = 0;
	MaxTimeToMove = (OrderDirection.Size() / GetMovementComponent()->GetMaxSpeed()) * 1.4;
	//1.4 is a magic number, just want to give units more time than they should need to make it to buildings, 
	//but also want to provide a timeout so they are stuck trying to get to a building that's surrounded 
	//and will fall out to idle and pick up a target that way.
	BuildingToAttack = Cast<ABuildingBase>(BuildingToTarget);
	if (EnemiesInRange.Contains(BuildingToAttack)) ReachedBuilding = true;
	else { ReachedBuilding = false; }
	ActiveCommandType = EProcessingCommandType::ORDER_BUILDING;
}

bool AUnitBase::MoveToBuilding_Validate(AActor* BuildingToTarget)
{
	return true;
}

float AUnitBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                            AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.0f)
	{
		SetLifeSpan(0.001f);
		//need to call for bloodsplat decal here :0
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::SanitizeFloat(Health));
	}
	return DamageAmount;
}


void AUnitBase::UpdateMaterial(FLinearColor PlayerColor)
{
	ColorUnitDecal(PlayerColor);
	ColorUnit(PlayerColor);
}

void AUnitBase::ColorUnitDecal_Implementation(FLinearColor PlayerColor)
{
	if (UnitDecalMaterialDynamic != nullptr && UnderUnitDecal != nullptr)
	{
		UnitDecalMaterialDynamic->SetVectorParameterValue(ColorParameterName, PlayerColor);
		UnderUnitDecal->SetDecalMaterial(UnitDecalMaterialDynamic);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "UnitBase ColorUnitDecal: decal doesn't exist");
	}
}

bool AUnitBase::ColorUnitDecal_Validate(FLinearColor PlayerColor)
{
	return true;
}

void AUnitBase::ColorUnit_Implementation(FLinearColor PlayerColor)
{
	if (UnitMeshMaterialDynamic) UnitMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
}

bool AUnitBase::ColorUnit_Validate(FLinearColor PlayerColor)
{
	return true;
}

void AUnitBase::SetDecalVisibility(bool bIsVisible)
{
	if (SelectionCircleDecal) SelectionCircleDecal->SetVisibility(bIsVisible);
}

float AUnitBase::GetHealth()
{
	return Health;
}

UClass* AUnitBase::GetProjectileClass()
{
	return ProjectileClass;
}

void AUnitBase::FixSpawnLocation_Implementation(FVector RealLocation)
{
	NetFixSpawnLocation(RealLocation);
}

bool AUnitBase::FixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

bool AUnitBase::IsUnitSelected() const
{
	return this->bIsUnitSelected;
}

void AUnitBase::SelectUnit(const bool bShouldSelect)
{
	this->bIsUnitSelected = bShouldSelect;

	if (UEmpire::IsEmpireValid(this->ControllingEmpire))
	{
		this->ControllingEmpire->SelectUnit(this, bShouldSelect);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Empire not valid for unit %s"), *GetNameSafe(this));
	}
}

void AUnitBase::NetFixSpawnLocation_Implementation(FVector RealLocation)
{
	SetRole(ROLE_Authority);
	SetActorLocation(RealLocation);
}

bool AUnitBase::NetFixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}
