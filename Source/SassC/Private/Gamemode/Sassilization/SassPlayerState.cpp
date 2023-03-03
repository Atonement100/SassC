// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Net/UnrealNetwork.h"

ASassPlayerState::ASassPlayerState()
{
}

UEmpire* ASassPlayerState::GetEmpire() const
{
	return Empire;
}

void ASassPlayerState::SetEmpire(UEmpire* const NewEmpire)
{
	this->Empire = NewEmpire;
}

void ASassPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASassPlayerState, PlayerColor);
	DOREPLIFETIME(ASassPlayerState, ControlledBuildings);
}
