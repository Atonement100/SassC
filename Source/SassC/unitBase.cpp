// Fill out your copyright notic in the Description page of Project Settings.

#include "SassC.h"
#include "buildingBase.h"
#include "sassPlayer.h"
#include "sassPlayerState.h"
#include "unitController.h"
#include "projectileSmallArrow.h"
#include "Net/UnrealNetwork.h"
#include "SassCStaticLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "unitBase.h"

AunitBase::AunitBase()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 30.0f;
	
	UnderUnitDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Under Unit Decal"));
	UnderUnitDecal->AttachTo(RootComponent);
	UnderUnitDecal->FadeScreenSize = 0.0f;
	UnderUnitDecal->SetRelativeLocationAndRotation(FVector(0, 0, -10.2f), FQuat(FRotator(-90.0f, 0, 0)));
	UnderUnitDecal->SetRelativeScale3D(FVector(10, 5, 5));
	UnderUnitDecal->DecalSize = FVector(1, 1, 1);

	SelectionCircleDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Circle Decal"));
	SelectionCircleDecal->AttachTo(RootComponent);
	SelectionCircleDecal->bVisible = false;
	SelectionCircleDecal->FadeScreenSize = 0.0f;
	SelectionCircleDecal->SetRelativeLocationAndRotation(FVector(0, 0, -9), FQuat(FRotator(-90.0f, 0, 0)));
	SelectionCircleDecal->SetRelativeScale3D(FVector(10, 5, 5));
	SelectionCircleDecal->DecalSize = FVector(1, 1, 1);

	DetectionSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Detection Sphere"));
	DetectionSphere->AttachTo(RootComponent);
	DetectionSphere->SetRelativeLocation(FVector(0, 0, -9.0f));
	DetectionSphere->SetRelativeScale3D(FVector(.32, .32, .2));
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AunitBase::OnOverlapBegin_DetectionSphere);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AunitBase::OnOverlapEnd_DetectionSphere);

	AggroSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Aggro Sphere"));
	AggroSphere->ComponentTags.Add(USassCStaticLibrary::NoAggroTag());
	AggroSphere->AttachTo(RootComponent);
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AunitBase::OnOverlapBegin_AggroSphere);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AunitBase::OnOverlapEnd_AggroSphere);
	AggroSphere->SetWorldScale3D(FVector(AttackRange / SelectionSphereScaleMod));
	//Decide if I want to have smaller aggro radius than attack range for idle characters... This would cause some issues with aggroing 
	//In situations where someone on the offensive sends units /near/ and enemy but does not click him. Maybe lower aggro radius after
	//A period of idle time to offset this?

	//TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));
	//TextRender->AttachTo(RootComponent);

	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	CharMoveComp->CrouchedHalfHeight = 5.0f;
	CharMoveComp->bOrientRotationToMovement = true;
	CharMoveComp->DefaultWaterMovementMode = EMovementMode::MOVE_Walking;
	CharMoveComp->MaxStepHeight = 0;
	CharMoveComp->MaxWalkSpeed = 50.0f;
	CharMoveComp->MaxWalkSpeedCrouched = 50.0f;

}

void AunitBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AunitBase, OwningPlayerID);
	DOREPLIFETIME(AunitBase, IsAttacking);
	DOREPLIFETIME(AunitBase, Health);
}

void AunitBase::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if (GetMesh()) { UnitMeshMaterialDynamic = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0)); }
}


void AunitBase::OnOverlapBegin_DetectionSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		if (PlayerStateRef != nullptr && this->OwningPlayerID == PlayerStateRef->PlayerId) {
			SetDecalVisibility(true);
		}
	}
}

void AunitBase::OnOverlapEnd_DetectionSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		if (PlayerStateRef != nullptr && this->OwningPlayerID == PlayerStateRef->PlayerId) {
			SetDecalVisibility(false);
		}
	}
}

void AunitBase::OnOverlapBegin_AggroSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor != this && !OtherComp->ComponentHasTag(USassCStaticLibrary::NoAggroTag())) {
		if (OtherActor->IsA(AunitBase::StaticClass()) && Cast<AunitBase>(OtherActor)->OwningPlayerID != OwningPlayerID) {
			EnemiesInRange.Add(OtherActor);
		}
		else if (OtherActor->IsA(AbuildingBase::StaticClass()) && Cast<AbuildingBase>(OtherActor)->OwningPlayerID != OwningPlayerID) {
			EnemiesInRange.Add(OtherActor);
			if (OtherActor == BuildingToAttack) { ReachedBuilding = true; }
		}
	}
}

void AunitBase::OnOverlapEnd_AggroSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (!OtherActor) return;
	EnemiesInRange.Remove(OtherActor);
	if (OtherActor == BuildingToAttack) { ReachedBuilding = false; }
}

void AunitBase::BeginPlay()
{
	Super::BeginPlay();
}

void AunitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeSinceAttack += DeltaTime;

	switch (ActiveCommandType) {
	case EProcessingCommandType::ORDER_IDLE:
		if (EnemiesInRange.Num() > 0 && TimeSinceAttack > AttackDelay && EnemiesInRange[0]) {
			if (AunitBase* Unit = Cast<AunitBase>(EnemiesInRange[0])) {
				if (Unit->OwningPlayerID != this->OwningPlayerID) {
					MoveToUnit(Unit, true);
				}
				else {
					EnemiesInRange.RemoveAt(0);
				}
			}
			else if (AbuildingBase* Bldg = Cast<AbuildingBase>(EnemiesInRange[0])) {
				if (Bldg->OwningPlayerID != this->OwningPlayerID) {
					MoveToBuilding(Bldg);
				}
				else {
					EnemiesInRange.RemoveAt(0);
				}
			}
		}
		else if (IsAttacking && EnemiesInRange.Num() == 0) {
			SetIsAttacking(false);
		}
		break;
	case EProcessingCommandType::ORDER_STATIC_UNIT:
		if (ActorToFollow) {
			OrderDirection = ActorToFollow->GetActorLocation() - GetActorLocation();
			if (OrderDirection.Size() < AttackRange) {
				SetActorRotation(FRotator(0,OrderDirection.Rotation().Yaw,0));
				if (TimeSinceAttack > AttackDelay){
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Static Attack!!");
					TimeSinceAttack = 0.0f;
					if (ActorToFollow->GetHealth() > 0) {
						Attack(ActorToFollow);
						if (!IsAttacking) SetIsAttacking(true);
					}
					else {
						ActorToFollow = nullptr;
						SwitchToIdle();
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, "Static Attack target has been killed");
					}
				}
			}
			else {
				ActorToFollow = nullptr;
				SwitchToIdle();
			}
		}
		else {
			SwitchToIdle();
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete");
		}
	
		break;
	case EProcessingCommandType::ORDER_WORLD: 
		AddMovementInput(OrderDirection, 1.0f);
		TimeSinceOrdered += DeltaTime;
		if ((OrderDestination - GetActorLocation()).Size2D() < 5.0f || TimeSinceOrdered > MaxTimeToMove) {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "UnitBase Movement Complete");
			SwitchToIdle();
		}
	
		break;
	case EProcessingCommandType::ORDER_UNIT: 
		if (ActorToFollow) {
			OrderDirection = ActorToFollow->GetActorLocation() - GetActorLocation();
			if (OrderDirection.Size() > AttackRange) AddMovementInput(OrderDirection, 1.0f);
			SetActorRotation(FRotator(0, OrderDirection.Rotation().Yaw, 0));

			if (OrderDirection.Size() < AttackRange && TimeSinceAttack > AttackDelay) { //No need to check if unit is friendly or hostile for this attack, as "ActorToFollow" can only ever be hostile.
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Attack!!");
				TimeSinceAttack = 0.0f;
				if (ActorToFollow->GetHealth() > 0) {
					Attack(ActorToFollow);
					if (!IsAttacking) SetIsAttacking(true);
				}
				else {
					ActorToFollow = nullptr;
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete (target has been killed)");
					SwitchToIdle();
				}
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete");
			SwitchToIdle();
		}
	
		break;
	case EProcessingCommandType::ORDER_BUILDING: 

		if (BuildingToAttack && BuildingToAttack->GetHealth() > 0) {
			if (!ReachedBuilding) {
				OrderDirection = BuildingToAttack->GetActorLocation() - this->GetActorLocation();
				AddMovementInput(OrderDirection, 1.0f);
				if (IsAttacking) SetIsAttacking(false);

				TimeSinceOrdered += DeltaTime;
				if (TimeSinceOrdered > MaxTimeToMove) {
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Unitbase could not move to building in time");
					SwitchToIdle();
				}
			}
			else if (TimeSinceAttack > AttackDelay) {
				if (BuildingToAttack->GetHealth() > 0) {
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "UnitBase: City attack");
					Attack(BuildingToAttack);
					if (!IsAttacking) SetIsAttacking(true);
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, "UnitBase Building Attack Complete (target has been killed)");
					BuildingToAttack = nullptr;
					SwitchToIdle();
				}
			}
		}
		else {
			SwitchToIdle();
		}
	
		break;
	default: 
		SwitchToIdle();
		break;
	}
}

void AunitBase::SwitchToIdle() {
	ActiveCommandType = EProcessingCommandType::ORDER_IDLE;
	if (IsAttacking) SetIsAttacking(false);

}

void AunitBase::Attack_Implementation(AActor* Target) {
	this->TimeSinceAttack = 0.0f;

	if (!Target) {
		SetIsAttacking(false);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "UnitBase Attack: Attack failed, target does not exist");
		return;
	}

	const FDamageEvent DamageInfo = FDamageEvent();
	Target->TakeDamage(this->AttackDamage, DamageInfo, nullptr, this);
	this->SetActorRotation(FRotator(0, UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Target->GetActorLocation()).Yaw, 0));
}

bool AunitBase::Attack_Validate(AActor* Target) {
	return true;
}

void AunitBase::SpawnProjectile_Implementation(FVector TargetLocation)
{
	//This and related functions are not expressed in unitBase because each projectile-shooting 
	//unit has the potential for very unreleated projectile logic.
	FVector TargetDisplacement = ((TargetLocation + FVector(0,0,this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())) - this->GetActorLocation());

	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	const FVector Location = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket"));
	const FRotator Rotation = TargetDisplacement.Rotation();

	AActor* Projectile = GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation, SpawnParams);
	FVector Velocity = Cast<AprojectileSmallArrow>(Projectile)->MovementComponent->Velocity;
	if (Projectile) Projectile->SetLifeSpan((TargetDisplacement).Size() / Velocity.Size());
}

bool AunitBase::SpawnProjectile_Validate(FVector TargetLocation)
{
	return true;
}


void AunitBase::StartAttackAnimation_Implementation() {
	this->IsAttacking = true;
}

bool AunitBase::StartAttackAnimation_Validate() {
	return true;
}

void AunitBase::StopAttackAnimation_Implementation() {
	this->IsAttacking = false;
}

bool AunitBase::StopAttackAnimation_Validate() {
	return true;
}

void AunitBase::SetIsAttacking_Implementation(bool NewIsAttacking)
{
	this->IsAttacking = NewIsAttacking;
}

bool AunitBase::SetIsAttacking_Validate(bool NewIsAttacking)
{
	return true;
}

void AunitBase::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void AunitBase::MoveToDest_Implementation(FVector Destination) {
	if (IsAttacking)SetIsAttacking(false);
	OrderDestination = Destination;
	OrderDirection = Destination - GetActorLocation();
	TimeSinceOrdered = 0;
	MaxTimeToMove = OrderDirection.Size() / GetMovementComponent()->GetMaxSpeed();
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase MaxTimeToMove = " + UKismetStringLibrary::Conv_FloatToString(MaxTimeToMove));
	ActiveCommandType = EProcessingCommandType::ORDER_WORLD;
}

bool AunitBase::MoveToDest_Validate(FVector Destination) {
	return true;
}

void AunitBase::MoveToUnit_Implementation(AActor* UnitToAttack, bool IsStaticAttack)
{
	if (IsAttacking)SetIsAttacking(false);
	ActorToFollow = Cast<AunitBase>(UnitToAttack);
	if (IsStaticAttack) {
		ActiveCommandType = EProcessingCommandType::ORDER_STATIC_UNIT;
	}
	else {
		ActiveCommandType = EProcessingCommandType::ORDER_UNIT;
	}
}

bool AunitBase::MoveToUnit_Validate(AActor* UnitToAttack, bool IsStaticAttack)
{
	return true;
}

void AunitBase::MoveToBuilding_Implementation(AActor* BuildingToTarget)
{
	if (IsAttacking)SetIsAttacking(false);
	OrderDirection = BuildingToTarget->GetActorLocation() - GetActorLocation();
	TimeSinceOrdered = 0;
	MaxTimeToMove = (OrderDirection.Size() / GetMovementComponent()->GetMaxSpeed()) * 1.4; 
	//1.4 is a magic number, just want to give units more time than they should need to make it to buildings, 
	//but also want to provide a timeout so they are stuck trying to get to a building that's surrounded 
	//and will fall out to idle and pick up a target that way.
	BuildingToAttack = Cast<AbuildingBase>(BuildingToTarget);
	if (EnemiesInRange.Contains(BuildingToAttack)) ReachedBuilding = true;
	else { ReachedBuilding = false; }
	ActiveCommandType = EProcessingCommandType::ORDER_BUILDING;
}
bool AunitBase::MoveToBuilding_Validate(AActor* BuildingToTarget)
{
	return true;
}

float AunitBase::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.0f) {
		SetLifeSpan(0.001f);
		//need to call for bloodsplat decal here :0
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, UKismetStringLibrary::Conv_FloatToString(Health));
	}
	return DamageAmount;
}


void AunitBase::UpdateMaterial(FLinearColor PlayerColor)
{
	ColorUnitDecal(PlayerColor);
	ColorUnit(PlayerColor);
}

void AunitBase::ColorUnitDecal_Implementation(FLinearColor PlayerColor) {
	if (UnitDecalMaterialDynamic != nullptr && UnderUnitDecal != nullptr) {
		UnitDecalMaterialDynamic->SetVectorParameterValue(ColorParameterName, PlayerColor);
		UnderUnitDecal->SetDecalMaterial(UnitDecalMaterialDynamic);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "UnitBase ColorUnitDecal: decal doesn't exist");
	}
}

bool AunitBase::ColorUnitDecal_Validate(FLinearColor PlayerColor) {
	return true;
}

void AunitBase::ColorUnit_Implementation(FLinearColor PlayerColor) {
	if (UnitMeshMaterialDynamic) UnitMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
}

bool AunitBase::ColorUnit_Validate(FLinearColor PlayerColor) {
	return true;
}

void AunitBase::SetDecalVisibility(bool isVisible) {
	if (SelectionCircleDecal) SelectionCircleDecal->SetVisibility(isVisible);
}

float AunitBase::GetHealth() {
	return Health;
}

void AunitBase::FixSpawnLocation_Implementation(FVector RealLocation)
{
	NetFixSpawnLocation(RealLocation);
}

bool AunitBase::FixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

void AunitBase::NetFixSpawnLocation_Implementation(FVector RealLocation)
{
	Role = ROLE_Authority;
	SetActorLocation(RealLocation);
}

bool AunitBase::NetFixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}
