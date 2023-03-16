// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/City.h"
#include "SassC.h"
#include "TimerManager.h"
#include "Gamemode/Sassilization/Empire.h"

ACity::ACity()
{
}

void ACity::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACity::BeginPlay()
{
	Super::BeginPlay();
}

void ACity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACity::PostCreation_Implementation(FLinearColor PlayerColor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "City has been spawned for a player");
}

bool ACity::PostCreation_Validate(FLinearColor PlayerColor)
{
	return true;
}

void ACity::TimedExpansion()
{
	UE_LOG(Sassilization, Display, TEXT("Timed expansion occuring for %s"), *this->GetName())
	this->Expand(FRotator(0, FMath::RandRange(0.f, 360.f), 0));

	FTimerHandle NextTimedExpansion;
	const FTimerDelegate NextTimedDelegate = FTimerDelegate().CreateLambda([this] { this->TimedExpansion(); });
	GetWorldTimerManager().SetTimer(NextTimedExpansion, NextTimedDelegate, FMath::RandRange(3.f, 10.f), false);
}

void ACity::WhenBuilt()
{
	Super::WhenBuilt();

	if (ControllingEmpire)
	{
		ControllingEmpire->SetNumCities(ControllingEmpire->GetNumCities() + 1);
		ControllingEmpire->CalculateSupply();
	}

	//TODO: Implement "Update Control"

	
	FTimerHandle RepeatingExpansion;
	const FTimerDelegate RepeatingDelegate = FTimerDelegate().CreateLambda([this] { this->TimedExpansion(); });
	GetWorldTimerManager().SetTimer(RepeatingExpansion, RepeatingDelegate, FMath::RandRange(3.f, 10.f), false);
	
	FTimerHandle Expansion1, Expansion2, Expansion3, Expansion4;
	const FTimerDelegate ExpansionDelegate = FTimerDelegate().CreateLambda([this] {this->Expand(FRotator(0, FMath::RandRange(0.f, 360.f), 0));});

	GetWorldTimerManager().SetTimer(Expansion1, ExpansionDelegate, FMath::RandRange(3.f, 10.f), false);
	GetWorldTimerManager().SetTimer(Expansion2, ExpansionDelegate, FMath::RandRange(3.f, 10.f), false);
	GetWorldTimerManager().SetTimer(Expansion3, ExpansionDelegate, FMath::RandRange(3.f, 10.f), false);
	GetWorldTimerManager().SetTimer(Expansion4, ExpansionDelegate, FMath::RandRange(3.f, 10.f), false);
	//TODO: Implement post-build work including expansions

	UE_LOG(Sassilization, Display, TEXT("Whenbuilt over"))
}
