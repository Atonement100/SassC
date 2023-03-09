// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemode/Sassilization/EmpireManager.h"
#include "Gamemode/Sassilization/SassGamemode.h"
#include "Gamemode/Sassilization/SassGameState.h"
#include "Player/SassPlayer.h"
#include "Player/SassPlayerController.h"
#include "Gamemode/Sassilization/SassPlayerState.h"

ASassGamemode::ASassGamemode()
{
}

void ASassGamemode::BeginPlay()
{
}

void ASassGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ASassPlayerState* const NewPlayerState = NewPlayer->GetPlayerState<ASassPlayerState>();

	this->GameManager = GetGameState<ASassGameState>()->GetGameManager();
	
	while(!GameManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameManager not yet ready"))
		FTimerHandle UnusedHandle;
		GetWorldTimerManager()
			.SetTimer(UnusedHandle, FTimerDelegate().CreateLambda([this](){}), 0.25f, false);
	}

	AEmpireManager* EmpireManager = this->GameManager->GetEmpireManager();
	
	while(!EmpireManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("EmpireManager not yet ready"))
		FTimerHandle UnusedHandle;
		GetWorldTimerManager()
			.SetTimer(UnusedHandle, FTimerDelegate().CreateLambda([this](){}), 0.25f, false);
		EmpireManager = this->GameManager->GetEmpireManager();
	}
	
	EmpireManager->ServerCreateNewEmpire(NewPlayerState->GetPlayerId(), NewPlayerState->GetPlayerName(), NewPlayerState);
}

/*Here, we want to assign players their color, and draw their HUD to catch them up if they are late*/
void ASassGamemode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

}
