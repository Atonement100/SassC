// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemode/Sassilization/SassPlayerState.h"

#include "Gamemode/Sassilization/SassGameState.h"
#include "Net/UnrealNetwork.h"

ASassPlayerState::ASassPlayerState()
{
}

void ASassPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ASassGameState* SassGameState = GetWorld() ? GetWorld()->GetGameState<ASassGameState>() : nullptr;

	if (!SassGameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("SassGameState not found by playerstate.."))
	}
	
	SassGameState->HandleNewPlayer(this);
}

void ASassPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASassPlayerState, PlayerColor);
	DOREPLIFETIME(ASassPlayerState, ControlledBuildings);
	DOREPLIFETIME(ASassPlayerState, Empire);
}

AEmpire* ASassPlayerState::GetEmpire() const
{
	if (!Empire)
	{
		UE_LOG(LogTemp, Warning, TEXT("Empire does not exist?!"))
	}
	
	return Empire;
}

void ASassPlayerState::SetEmpire(AEmpire* const NewEmpire)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting empire to %s"), NewEmpire ? *NewEmpire->GetName() : TEXT("NULL"))
	
	this->Empire = NewEmpire;
}

ETypeOfSpawnable ASassPlayerState::GetSelectedTypeOfSpawnable() const
{
	return SelectedTypeOfSpawnable;
}

void ASassPlayerState::SetSelectedTypeOfSpawnable(const ETypeOfSpawnable NewSelectedTypeOfSpawnable)
{
	this->SelectedTypeOfSpawnable = NewSelectedTypeOfSpawnable;
}

ETypeOfBuilding ASassPlayerState::GetSelectedTypeOfBuilding() const
{
	return SelectedTypeOfBuilding;
}

void ASassPlayerState::SetSelectedTypeOfBuilding(const ETypeOfBuilding NewSelectedTypeOfBuilding)
{
	this->SelectedTypeOfBuilding = NewSelectedTypeOfBuilding;
}

ETypeOfUnit ASassPlayerState::GetSelectedTypeOfUnit() const
{
	return SelectedTypeOfUnit;
}

void ASassPlayerState::SetSelectedTypeOfUnit(const ETypeOfUnit NewSelectedTypeOfUnit)
{
	this->SelectedTypeOfUnit = NewSelectedTypeOfUnit;
}

void ASassPlayerState::UpdateBuildingSelection(const ETypeOfBuilding NewTypeOfBuilding)
{
	this->SelectedTypeOfBuilding = NewTypeOfBuilding;
	this->UpdateSpawnableTypeSelection(ETypeOfSpawnable::Building);
}

void ASassPlayerState::UpdateUnitSelection(const ETypeOfUnit NewTypeOfUnit)
{
	this->SelectedTypeOfUnit = NewTypeOfUnit;
	this->UpdateSpawnableTypeSelection(ETypeOfSpawnable::Unit);
}

void ASassPlayerState::UpdateSpawnableTypeSelection(const ETypeOfSpawnable NewTypeOfSpawnable)
{
	this->SelectedTypeOfSpawnable = NewTypeOfSpawnable;
}