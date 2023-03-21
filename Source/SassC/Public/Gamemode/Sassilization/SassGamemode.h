// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "SassGameManager.h"
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
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gamemode")
	TArray<AActor*> PlayerSpawns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamemode")
	ASassGameManager* GameManager;

	UFUNCTION(BlueprintCallable, Category = "Gamemode")
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
