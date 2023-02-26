// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/GameMode.h"
#include "SassGamemode.generated.h"

class ASassPlayerController;

/**
 * 
 */
UCLASS()
class SASSC_API ASassGamemode : public AGameMode
{
	GENERATED_BODY()
	ASassGamemode();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gamemode")
	TArray<AActor*> PlayerSpawns;

	UFUNCTION(BlueprintCallable, Category = "Gamemode")
	void PostLogin(APlayerController* NewPlayer);
	//	UFUNCTION(BlueprintCallable, Category = "Gamemode")
	//		AActor* FindPlayerStart(AsassPlayerController* Player, FString IncomingName);
	UFUNCTION(BlueprintCallable, Category = "Gamemode")
	FLinearColor ChoosePlayerColor();
};
