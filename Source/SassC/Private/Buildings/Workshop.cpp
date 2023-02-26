// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Workshop.h"
#include "SassC.h"

AWorkshop::AWorkshop()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorkshop::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AWorkshop::BeginPlay()
{
	Super::BeginPlay();
}

void AWorkshop::Tick(const float DeltaSeconds)
{
	//todo this kind of thing probably doesnt need to be in tick (can Event?)
	Super::Tick(DeltaSeconds);
	if (PreviewActive) { PreviewUpgrade(); }
	else if (ResetRequired) { ResetPreview(); }
}
