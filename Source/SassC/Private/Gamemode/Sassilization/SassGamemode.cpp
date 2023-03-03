// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemode/Sassilization/SassGamemode.h"

#include "Gamemode/Sassilization/SassGameState.h"
#include "Player/SassPlayer.h"
#include "Player/SassPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gamemode/Sassilization/SassPlayerState.h"

ASassGamemode::ASassGamemode()
{
}

/*Here, we want to assign players their color, and draw their HUD to catch them up if they are late*/
void ASassGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ASassPlayerState* PlayerState = Cast<ASassPlayerState>(NewPlayer->PlayerState);

	if (ASassGameState* SassGameState = GetGameState<ASassGameState>())
	{
		SassGameState->HandleNewPlayer(PlayerState);
	}
}

FLinearColor ASassGamemode::ChoosePlayerColor()
{
	return UKismetMathLibrary::MakeColor(
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		1.0f);
}
