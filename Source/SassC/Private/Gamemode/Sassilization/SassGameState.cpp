// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemode/Sassilization/SassGameState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SassPlayer.h"
#include "Gamemode/Sassilization/SassGameManager.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Player/SassPlayerController.h"

ASassGameState::ASassGameState()
{

}

void ASassGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASassGameState, TimeKeeper);
	DOREPLIFETIME(ASassGameState, WarmUpTime);
	DOREPLIFETIME(ASassGameState, WinnerDeclared);
	DOREPLIFETIME(ASassGameState, GameStatus);
	DOREPLIFETIME(ASassGameState, GoldGoal);
	DOREPLIFETIME(ASassGameState, SassGameManager);
}

void ASassGameState::Tick(float DeltaSeconds)
{
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Turquoise,
	                                 FString::SanitizeFloat(TimeKeeper));
	if (GameStatus == ESassGameStatus::PreGame)
	{
		TimeKeeper += DeltaSeconds;
		if (TimeKeeper > WarmUpTime)
		{
			UE_LOG(LogTemp, Display, TEXT("GameState warm up time over. Game starting..."))
			GameStatus = ESassGameStatus::GameActive;
			GameStart();
		}
	}
}

ASassGameManager* ASassGameState::GetGameManager()
{
	if (!this->SassGameManager)
	{
		UE_LOG(LogTemp, Display, TEXT("SassGameManager does not already exist. Searching :)"))
		TArray<AActor*>&& GameManagers = TArray<AActor*>();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASassGameManager::StaticClass(), GameManagers);

		if (GameManagers.Num() != 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid number of SassGameManager found in the world. Found {%d}"), GameManagers.Num());
		}

		this->SassGameManager = Cast<ASassGameManager>(GameManagers[0]);
	}
	
	return this->SassGameManager;
}

void ASassGameState::GameStart_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("GameStart called"))

	ASassPlayerController* PlayerController = Cast<ASassPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	PlayerController->CreateGameHUD();
	
	//Player->GetGameWidget(); Cast to SassilizationHUD_BP; Player->SetSassHUDWidget()
	for (TActorIterator<ASassPlayer> PlayerItr(GetWorld()); PlayerItr; ++PlayerItr)
	{
		if(!PlayerItr)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerIterator did not exist in GameStart Impl"))
		}

		if (ASassPlayerState* PlayerState = PlayerItr->GetPlayerState<ASassPlayerState>())
		{
			UE_LOG(LogTemp, Display, TEXT("Setting color of player %s on %s's device."),
				*PlayerItr->GetPlayerState()->GetPlayerName(), *PlayerController->GetPlayerState<ASassPlayerState>()->GetPlayerName());
			
			if (PlayerState->GetEmpire())
			{
				PlayerItr->ColorPlayer(PlayerState->GetEmpire()->GetColor());
			}
		}
	}
}

bool ASassGameState::GameStart_Validate()
{
	return true;
}

void ASassGameState::LateStart_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("LateStart implementation called"))
	GameStart();
}

bool ASassGameState::LateStart_Validate()
{
	return true;
}

ESassGameStatus ASassGameState::GetGameStatus() const
{
	return GameStatus;
}

void ASassGameState::HandleNewPlayer_Implementation(ASassPlayerState* PlayerState)
{
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState not found?"));
		return;
	}
	
	const int32 PlayerId = PlayerState->GetPlayerId();
	const FString PlayerName = PlayerState->GetPlayerName();
	//was previously creating here as well..
	// AEmpire* NewEmpire = GetGameManager()->GetEmpireManager()->GetEmpireByPlayerId(PlayerId);
	//
	// if (!NewEmpire)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Empire not found for new player %s"), PlayerState ? *PlayerName : TEXT("UNKNOWNNAME"))
	// }
	//
	// PlayerState->SetEmpire(NewEmpire);
}

bool ASassGameState::HandleNewPlayer_Validate(ASassPlayerState* PlayerState)
{
	return true;
}
