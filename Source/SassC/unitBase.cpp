// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "unitBase.h"


// Sets default values
AunitBase::AunitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AunitBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AunitBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AunitBase::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

