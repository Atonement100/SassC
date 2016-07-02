// Fill out your copyright notic in the Description page of Project Settings.

#include "SassC.h"
#include "buildingBase.h"
#include "sassPlayer.h"
#include "sassPlayerState.h"
#include "unitController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "unitBase.h"

AunitBase::AunitBase()
{
	PrimaryActorTick.bCanEverTick = true;

	UnderUnitDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Under Unit Decal"));
	UnderUnitDecal->AttachTo(RootComponent);
	
	SelectionCircleDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Circle Decal"));
	SelectionCircleDecal->AttachTo(RootComponent);
	SelectionCircleDecal->bVisible = false;

	SelectionBlendDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Blend Decal"));
	SelectionBlendDecal->AttachTo(RootComponent);

	DetectionSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Detection Sphere"));
	DetectionSphere->AttachTo(RootComponent);

	AggroSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Aggro Sphere"));
	AggroSphere->ComponentTags.Add(NoAggroTag);
	AggroSphere->AttachTo(RootComponent);

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));
	TextRender->AttachTo(RootComponent);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AunitBase::OnOverlapBegin_DetectionSphere);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AunitBase::OnOverlapEnd_DetectionSphere);

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AunitBase::OnOverlapBegin_AggroSphere);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AunitBase::OnOverlapEnd_AggroSphere);
	AggroSphere->SetWorldScale3D(FVector(AttackRange / SelectionSphereScaleMod));
	//Decide if I want to have smaller aggro radius than attack range for idle characters... This would cause some issues with aggroing 
	//In situations where someone on the offensive sends units /near/ and enemy but does not click him. Maybe lower aggro radius after
	//A period of idle time to offset this?
}

void AunitBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AunitBase, OwningPlayerID);
	DOREPLIFETIME(AunitBase, IsAttacking);
}

void AunitBase::OnOverlapBegin_DetectionSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		if (PlayerStateRef != nullptr && PlayerStateRef->ControlledBuildings.Contains(this)){
			SetDecalVisibility(true);
		}
	}
}

void AunitBase::OnOverlapEnd_DetectionSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		if (PlayerStateRef != nullptr && PlayerStateRef->ControlledBuildings.Contains(this)) {
			SetDecalVisibility(false);
		}
	}
}

void AunitBase::OnOverlapBegin_AggroSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//need to add friendly unit check
	if (OtherActor != this && !OtherComp->ComponentHasTag(NoAggroTag) && ((OtherActor->IsA(AunitBase::StaticClass()) && Cast<AunitBase>(OtherActor)->OwningPlayerID != OwningPlayerID) || (OtherActor->IsA(AbuildingBase::StaticClass()) && OwningPlayerID != OwningPlayerID))) {
		EnemiesInRange.Add(OtherActor);
		if (OtherActor == BuildingToAttack) {
			ReachedBuilding = true;
		}
	}
}

void AunitBase::OnOverlapEnd_AggroSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (EnemiesInRange.Contains(OtherActor)) {
		EnemiesInRange.Remove(OtherActor);
		if (OtherActor == BuildingToAttack) {
			ReachedBuilding = false;
		}
	}
}

void AunitBase::BeginPlay()
{
	Super::BeginPlay();
}

void AunitBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	TimeSinceAttack += DeltaTime;
	if (ProcessingMoveToWorldOrder) {
		AddMovementInput(OrderDirection, 1.0f);
		TimeSinceOrdered += DeltaTime;
		if ((OrderDestination - GetActorLocation()).Size2D() < 5.0f || TimeSinceOrdered > MaxTimeToMove){
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase Movement Complete");
			ProcessingMoveToWorldOrder = false;
			IsAttacking = false;
		}
	}
	else if (ProcessingMoveToUnitOrder) {
		if (ActorToFollow) {
			OrderDirection = ActorToFollow->GetActorLocation() - GetActorLocation();
			AddMovementInput(OrderDirection, 1.0f);

			if (OrderDirection.Size() < AttackRange && TimeSinceAttack > AttackDelay) { //No need to check if unit is friendly or hostile for this attack, as "ActorToFollow" can only ever be hostile.
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Attack!!");
				TimeSinceAttack = 0.0f;
				
				const FDamageEvent DamageInfo = FDamageEvent();
				if (Cast<AunitBase>(ActorToFollow)->GetHealth() > 0) {
					ActorToFollow->TakeDamage(AttackDamage, DamageInfo, nullptr, this);
					IsAttacking = true;
				}
				else { 
					ActorToFollow = nullptr;
					ProcessingMoveToUnitOrder = false;
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete (target has been killed)");
					IsAttacking = false;
				}
			}
		}
		else {
			ProcessingMoveToUnitOrder = false;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete");
			IsAttacking = false;
		}
	}
	else if (ProcessingMoveToBuildingOrder) {
		if (BuildingToAttack) {
			if (!ReachedBuilding) {
				AddMovementInput(OrderDirection, 1.0f);
				IsAttacking = false;
			}
			else if (TimeSinceAttack > AttackDelay) {
				if (Cast<AbuildingBase>(BuildingToAttack)->GetHealth() > 0) {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "City attack");
					const FDamageEvent DamageInfo = FDamageEvent();
					BuildingToAttack->TakeDamage(AttackDamage, DamageInfo, nullptr, this);
					SetActorRotation(FRotator(0, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemiesInRange[0]->GetActorLocation()).Yaw, 0));
					TimeSinceAttack = 0.0f;
					IsAttacking = true;
				}
				else {
					IsAttacking = false;
					ProcessingMoveToBuildingOrder = false;
					BuildingToAttack = nullptr;
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "UnitBase Building Attack Complete (target has been killed)");
				}
			}
		}
		else {
			IsAttacking = false;
		}
	}
	else {
		if (EnemiesInRange.Num() > 0 && TimeSinceAttack > AttackDelay && EnemiesInRange[0]) {
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Proximity attack");
				const FDamageEvent DamageInfo = FDamageEvent();
				EnemiesInRange[0]->TakeDamage(AttackDamage, DamageInfo, nullptr, this); 
				SetActorRotation(FRotator(0,UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemiesInRange[0]->GetActorLocation()).Yaw,0));
				TimeSinceAttack = 0.0f;
				IsAttacking = true;
		}
		else if (EnemiesInRange.Num() == 0){
			IsAttacking = false;
		}
	}
}

void AunitBase::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void AunitBase::MoveToDest_Implementation(FVector Destination) {
	OrderDestination = Destination;
	OrderDirection = Destination - GetActorLocation();
	TimeSinceOrdered = 0;
	MaxTimeToMove = OrderDirection.Size() / GetMovementComponent()->GetMaxSpeed();
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase MaxTimeToMove = " + UKismetStringLibrary::Conv_FloatToString(MaxTimeToMove));
	ProcessingMoveToWorldOrder = true;
	ProcessingMoveToUnitOrder = false;
	ProcessingMoveToBuildingOrder = false;
}

bool AunitBase::MoveToDest_Validate(FVector Destination) {
	return true;
}

void AunitBase::MoveToUnit_Implementation(AActor* UnitToAttack)
{
	ActorToFollow = UnitToAttack;
	ProcessingMoveToUnitOrder = true;
	ProcessingMoveToWorldOrder = false;
	ProcessingMoveToBuildingOrder = false;
}

bool AunitBase::MoveToUnit_Validate(AActor* UnitToAttack)
{
	return true;
}

void AunitBase::MoveToBuilding_Implementation(AActor* BuildingToTarget)
{
	OrderDirection = BuildingToTarget->GetActorLocation() - GetActorLocation();
	BuildingToAttack = BuildingToTarget;
	ProcessingMoveToBuildingOrder = true;
	ProcessingMoveToUnitOrder = false;
	ProcessingMoveToWorldOrder = false;
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
}

void AunitBase::ColorUnitDecal_Implementation(FLinearColor PlayerColor) {
	if (UnitDecalMaterialDynamic != nullptr && UnderUnitDecal != nullptr) {
		UnitDecalMaterialDynamic->SetVectorParameterValue(ColorParameterName, PlayerColor);
		UnderUnitDecal->SetDecalMaterial(UnitDecalMaterialDynamic);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "decal doesn't exist");
	}
}

bool AunitBase::ColorUnitDecal_Validate(FLinearColor PlayerColor) {
	return true;
}

void AunitBase::SetDecalVisibility(bool isVisible) {
	if (SelectionCircleDecal) SelectionCircleDecal->SetVisibility(isVisible);
}

float AunitBase::GetHealth() {
	return Health;
}