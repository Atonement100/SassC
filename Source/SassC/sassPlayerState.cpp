// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "UnrealNetwork.h"
#include "sassPlayerState.h"

AsassPlayerState::AsassPlayerState() {

}

void AsassPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AsassPlayerState, PlayerColor);
}

TArray<AActor*> AsassPlayerState::GetControlledBuildings() const {
	return ControlledBuildings;
}