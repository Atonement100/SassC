// Fill out your copyright notic in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
#include "sassPlayerState.h"
#include "unitController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
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

	DetectionSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Detection Sphere"));
	DetectionSphere->AttachTo(RootComponent);

	AggroSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Aggro Sphere"));
	AggroSphere->AttachTo(RootComponent);

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));
	TextRender->AttachTo(RootComponent);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AunitBase::OnOverlapBegin_DetectionSphere);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AunitBase::OnOverlapEnd_DetectionSphere);

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AunitBase::OnOverlapBegin_AggroSphere);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AunitBase::OnOverlapEnd_AggroSphere);
	AggroSphere->SetWorldScale3D(FVector(AttackRange / 100));
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

}

void AunitBase::OnOverlapEnd_AggroSphere(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

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
		}
	}
	else if (ProcessingMoveToUnitOrder) {
		if (ActorToFollow) {
			OrderDirection = ActorToFollow->GetActorLocation() - GetActorLocation();
			AddMovementInput(OrderDirection, 1.0f);

			if (OrderDirection.Size() < AttackRange && TimeSinceAttack > AttackDelay) {
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Attack!!");
				TimeSinceAttack = 0.0f;
				//play animation, audio queue
				const FDamageEvent DamageInfo = FDamageEvent();

				ActorToFollow->TakeDamage(AttackDamage, DamageInfo, nullptr, this);
			}
		}
		else {
			ProcessingMoveToUnitOrder = false;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete");
		}
	}
	else {

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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase MaxTimeToMove = " + UKismetStringLibrary::Conv_FloatToString(MaxTimeToMove));
	ProcessingMoveToWorldOrder = true;
	ProcessingMoveToUnitOrder = false;
}

bool AunitBase::MoveToDest_Validate(FVector Destination) {
	return true;
}

void AunitBase::MoveToUnit_Implementation(AActor* UnitToAttack)
{
	ActorToFollow = UnitToAttack;
	ProcessingMoveToUnitOrder = true;
	ProcessingMoveToWorldOrder = false;
}

bool AunitBase::MoveToUnit_Validate(AActor* UnitToAttack)
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