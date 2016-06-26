// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
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
	AsassGameState* SassGameStateRef = Cast<AsassGameState>(GameState);
	if (!SassGameStateRef->PreGameActive) {
		SassGameStateRef->GameStart();
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, PlayerState->PlayerName + "login success, color set");
}

FLinearColor AsassGamemode::ChoosePlayerColor()
{
	return UKismetMathLibrary::MakeColor(
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		UKismetMathLibrary::RandomFloatInRange(0, 1),
		UKismetMathLibrary::RandomFloatInRange(0, 1));
}

