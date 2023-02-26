// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Tower.h"
#include "SassC.h"

ATower::ATower()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATower::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATower::BeginPlay()
{
	Super::BeginPlay();
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PreviewActive) { PreviewUpgrade(); }
	else if (ResetRequired) { ResetPreview(); }
}

