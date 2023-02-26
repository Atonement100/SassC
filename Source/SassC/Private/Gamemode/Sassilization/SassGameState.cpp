// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemode/Sassilization/SassGameState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SassPlayer.h"
#include "Gamemode/Sassilization/SassPlayerState.h"

ASassGameState::ASassGameState()
{
}

void ASassGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASassGameState, TimeKeeper);
	DOREPLIFETIME(ASassGameState, WarmUpTime);
	DOREPLIFETIME(ASassGameState, WinnerDeclared);
	DOREPLIFETIME(ASassGameState, PreGameActive);
	DOREPLIFETIME(ASassGameState, GoldGoal);
}

void ASassGameState::Tick(float DeltaSeconds)
{
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Turquoise,
	                                 FString::SanitizeFloat(TimeKeeper));
	if (PreGameActive)
	{
		TimeKeeper += DeltaSeconds;
		if (TimeKeeper > WarmUpTime)
		{
			PreGameActive = false;
			GameStart();
		}
	}
}

void ASassGameState::GameStart_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "Gamestart Called");
	
	ASassPlayer* Player = Cast<ASassPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->CreateGameHUD();

	//Player->GetGameWidget(); Cast to SassilizationHUD_BP; Player->SetSassHUDWidget()
	for (TActorIterator<ASassPlayer> PlayerItr(GetWorld()); PlayerItr; ++PlayerItr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "PlayerIterator");
		PlayerItr->ColorPlayer(PlayerItr->GetPlayerState<ASassPlayerState>()->PlayerColor);
	}
}

bool ASassGameState::GameStart_Validate()
{
	return true;
}

void ASassGameState::LateStart_Implementation()
{
	GameStart();
}

bool ASassGameState::LateStart_Validate()
{
	return true;
}
