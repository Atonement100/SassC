// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
#include "sassPlayerState.h"
#include "unitController.h"
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

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));
	TextRender->AttachTo(RootComponent);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AunitBase::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AunitBase::OnOverlapEnd);

}

void AunitBase::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		if (PlayerStateRef != nullptr && PlayerStateRef->ControlledBuildings.Contains(this)){
			SetDecalVisibility(true);
		}
	}

}

void AunitBase::OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		if (PlayerStateRef != nullptr && PlayerStateRef->ControlledBuildings.Contains(this)) {
			SetDecalVisibility(false);
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
		}
		else {
			ProcessingMoveToUnitOrder = false;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Silver, "UnitBase Unit Attack Chase Complete");
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
