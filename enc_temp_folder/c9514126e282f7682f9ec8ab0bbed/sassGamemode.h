// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "sassGamemode.generated.h"

class AsassPlayerController;

/**
 * 
 */
UCLASS()
class SASSC_API AsassGamemode : public AGameMode
{
	GENERATED_BODY()
	AsassGamemode();

public:
	//UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Gamemode")
	float WarmUpTime = 10.0f;
	//UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Gamemode")
	int GoldGoal = 800;
	//UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Gamemode")
	float TimeKeeper = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Gamemode")
		TArray<AActor*> PlayerSpawns;
	UFUNCTION(BlueprintCallable, Category = "Gamemode")
		AActor* FindPlayerStart(AsassPlayerController* Player, FString IncomingName);
	
};
