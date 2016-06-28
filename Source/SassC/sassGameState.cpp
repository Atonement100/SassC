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
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "Gamestart Called");


	AsassPlayer* Player = Cast<AsassPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->CreateGameHUD();

	//Player->GetGameWidget(); Cast to SassilizationHUD_BP; Player->SetSassHUDWidget()
	for (TActorIterator<AsassPlayer> PlayerItr(GetWorld()); PlayerItr; ++PlayerItr) {
		PlayerItr->ColorPlayer((Cast<AsassPlayerState>(PlayerItr->PlayerState))->PlayerColor);
	}
}

bool AsassGameState::GameStart_Validate() {
	return true;
}

void AsassGameState::LateStart_Implementation()
{
	GameStart();
}

bool AsassGameState::LateStart_Validate()
{
	return true;
}
