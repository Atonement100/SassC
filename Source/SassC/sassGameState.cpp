// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "sassPlayer.h"
#include "sassPlayerState.h"
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
	DOREPLIFETIME(AsassGameState, GoldGoal);

}

void AsassGameState::Tick(float DeltaSeconds) {
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Turquoise, UKismetStringLibrary::Conv_FloatToString(TimeKeeper));
	if (PreGameActive) {
		TimeKeeper += DeltaSeconds;
		if (TimeKeeper > WarmUpTime) {
			PreGameActive = false;
			GameStart();
		}
	}
}

void AsassGameState::GameStart_Implementation() {
	
	AsassPlayer* Player = Cast<AsassPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->CreateGameHUD();
	//Player->GetGameWidget(); Cast to SassilizationHUD_BP; Player->SetSassHUDWidget()
	for (TActorIterator<AsassPlayer> Player(GetWorld()); Player; ++Player) {
		Player->ColorPlayer((Cast<AsassPlayerState>(Player->PlayerState))->PlayerColor);
	}
}

bool AsassGameState::GameStart_Validate() {
	return true;
}
