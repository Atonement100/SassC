// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "unitBase.h"

AunitBase::AunitBase()
{
	PrimaryActorTick.bCanEverTick = true;



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

