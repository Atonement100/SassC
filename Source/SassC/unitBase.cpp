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
			SetDecalVisibility(SelectionCircleDecal, true);
		}
	}

}

void AunitBase::OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerCharacterRef != nullptr && OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		if (PlayerStateRef != nullptr && PlayerStateRef->ControlledBuildings.Contains(this)) {
			SetDecalVisibility(SelectionCircleDecal, false);
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
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, UKismetStringLibrary::Conv_VectorToString(GetActorLocation()));
}

void AunitBase::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AunitBase::MoveToDest_Implementation(FVector Destination) {
	
	if (AAIController* Controller = Cast<AAIController>(GetController())) {
			
		EPathFollowingRequestResult::Type PathingResult = Controller->MoveToLocation(Destination, 2.0f, true, false, true, true, 0, true);
		if (PathingResult == EPathFollowingRequestResult::Failed) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase MoveToLocation Dispatch Failed");
		}
		else if (PathingResult == EPathFollowingRequestResult::AlreadyAtGoal) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase MoveToLocation Already at goal");
		}
		else if (PathingResult == EPathFollowingRequestResult::RequestSuccessful) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "UnitBase MoveToLocation Dispatch success");
		}

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "UnitBase MoveToLocation: Cast to aicontroller failed");
	}
	/*
	FVector Direction = Destination - GetActorLocation();
	for (int i = 0; i < 100000; i++) {
		AddMovementInput(Direction, 1.0f);
	}
	*/
}

bool AunitBase::MoveToDest_Validate(FVector Destination) {
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

void AunitBase::SetDecalVisibility(UDecalComponent* DecalRef, bool isVisible) {
	//@TODO:
	//change this function to only intake bool and automatically get self underunitdecal reference
	//propagate this change to remaining calls of the function
	if (DecalRef != nullptr) DecalRef->SetVisibility(isVisible);
	else { GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, "decal doesn't exist (visibility)"); }
}
