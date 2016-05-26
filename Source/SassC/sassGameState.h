// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "sassGameState.generated.h"


UCLASS()
class SASSC_API AsassGameState : public AGameState
{
	GENERATED_BODY()

	AsassGameState();
	
public:

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
		float TimeKeeper = 0.0f;
	UPROPERTY(Replicated, BlueprintReadWrite)
		float WarmUpTime = 10.0f;
	UPROPERTY(Replicated, BlueprintReadWrite)
		bool WinnerDeclared = false;
	UPROPERTY(Replicated, BlueprintReadWrite)
		bool PreGameActive = true;

	//virtual UWorld* GetWorld() const override;
	//UPROPERTY(Transient)
	//UWorld* World;

/*
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void GameStart();
	virtual void GameStart_Implementation();
	virtual bool GameStart_Validate();
	*/
	
	};
