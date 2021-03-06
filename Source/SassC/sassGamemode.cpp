// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
#include "sassPlayerController.h"
#include "sassPlayerState.h"
#include "sassGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "sassGamemode.h"

AsassGamemode::AsassGamemode() {

}

/*Here, we want to assign players their color, and draw their HUD to catch them up if they are late*/
void AsassGamemode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	AsassPlayerState* PlayerState = Cast<AsassPlayerState>(NewPlayer->PlayerState);
	PlayerState->PlayerColor = ChoosePlayerColor();
}

FLinearColor AsassGamemode::ChoosePlayerColor()
{
	return UKismetMathLibrary::MakeColor(
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		1.0f);
}

