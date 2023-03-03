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
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "Gamestart Called");
	
	ASassPlayer* Player = Cast<ASassPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->CreateGameHUD();

	//Player->GetGameWidget(); Cast to SassilizationHUD_BP; Player->SetSassHUDWidget()
	for (TActorIterator<ASassPlayer> PlayerItr(GetWorld()); PlayerItr; ++PlayerItr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "Setting color of player..");
		UE_LOG(LogTemp, Display, TEXT("Setting color of player %s"), *(PlayerItr->GetPlayerState()->GetPlayerName()));

		if (ASassPlayerState* PlayerState = PlayerItr->GetPlayerState<ASassPlayerState>())
		{
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
	GameStart();
}

bool ASassGameState::LateStart_Validate()
{
	return true;
}

void ASassGameState::HandleNewPlayer_Implementation(ASassPlayerState* PlayerState)
{
	UEmpire* NewEmpire = GetGameManager()->GetEmpireManager()->RetrieveOrCreateNewEmpire(PlayerState->GetPlayerId(), PlayerState->GetPlayerName());

	if (!NewEmpire)
	{
		UE_LOG(LogTemp, Warning, TEXT("Empire not found for new player %s"), *PlayerState->GetPlayerName())
	}
	
	PlayerState->SetEmpire(NewEmpire);
}

bool ASassGameState::HandleNewPlayer_Validate(ASassPlayerState* PlayerState)
{
	return true;
}
