// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "sassPlayer.h"
#include "sassGameState.h"

AsassGameState::AsassGameState() {

}

void AsassGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AsassGameState, TimeKeeper);
	DOREPLIFETIME(AsassGameState, WarmUpTime);
	DOREPLIFETIME(AsassGameState, WinnerDeclared);
	DOREPLIFETIME(AsassGameState, PreGameActive);

}

/*
UWorld* AsassGameState::GetWorld() const {
	return World;
}
*/
void AsassGameState::Tick(float DeltaSeconds) {
	if (PreGameActive) {
		TimeKeeper += DeltaSeconds;
		if (TimeKeeper > WarmUpTime) {
			PreGameActive = false;
			//GameStart();
		}
	}
}

/*
void GameStart_Implementation() {
	if (World == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "World null"); 
		return; 
	}
	for (TActorIterator<AsassPlayer> Itr(World); Itr; ++Itr) {

	}
}

bool GameStart_Validate() {
	return true;
}
*/