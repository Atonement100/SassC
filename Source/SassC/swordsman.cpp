// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "swordsman.h"

Aswordsman::Aswordsman()
{
	PrimaryActorTick.bCanEverTick = true;

	/*
	UnitMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Unit Mesh"));
	UnitMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Blueprints/sassilization/UnitModels/swordsman/swordsman.swordsman'"));
	if (TempMesh.Succeeded()) { UnitMesh->SetSkeletalMesh(TempMesh.Object); }
	*/ 
	// Mesh is inherited from character, might not need to replace it.

	UnderUnitDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Under Unit Decal"));
	UnderUnitDecal->AttachTo(RootComponent);

	SelectionCircleDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Circle Decal"));
	SelectionCircleDecal->AttachTo(RootComponent);
	SelectionCircleDecal->bVisible = false;

	DetectionSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Detection Sphere"));
	DetectionSphere->AttachTo(RootComponent);

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));
	TextRender->AttachTo(RootComponent);
}

void Aswordsman::BeginPlay()
{
	Super::BeginPlay();

}

void Aswordsman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void Aswordsman::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}




