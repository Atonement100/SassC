// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
#include "sassPlayerState.h"
#include "Kismet/GameplayStatics.h"
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

	if (OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		
		//if Playerstate->ControllerBuildings contains self, turn on decal of unit
	}

}

void AunitBase::OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	AsassPlayer* PlayerCharacterRef = (AsassPlayer*)UGameplayStatics::GetPlayerCharacter(this, 0);

	if (OtherActor == PlayerCharacterRef->GetSelectionSphereHolder()) {
		AsassPlayerState* PlayerStateRef = (AsassPlayerState*)PlayerCharacterRef->PlayerState;
		//if Playerstate->ControllerBuildings contains self, turn off decal of unit
	}
}

void AunitBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AunitBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AunitBase::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AunitBase::UpdateMaterial(FLinearColor PlayerColor)
{
	ColorUnitDecal(PlayerColor);
}

void AunitBase::ColorUnitDecal_Implementation(FLinearColor PlayerColor) {
	UnitDecalMaterialDynamic->SetVectorParameterValue(ColorParameterName, PlayerColor);
	UnderUnitDecal->SetDecalMaterial(UnitDecalMaterialDynamic);
}

bool AunitBase::ColorUnitDecal_Validate(FLinearColor PlayerColor) {
	return true;
}

void AunitBase::SetDecalVisiblity(UDecalComponent* DecalRef, bool isVisible) {
	
}

