// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "SassGameManager.h"
#include "GameFramework/GameState.h"
#include "SassGameState.generated.h"


UCLASS()
class SASSC_API ASassGameState : public AGameState
{
	GENERATED_BODY()

	ASassGameState();

public:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(Replicated, BlueprintReadWrite)
	float TimeKeeper = 0.0f;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float WarmUpTime = 10.0f;
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool WinnerDeclared = false;
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool PreGameActive = true;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float GoldGoal = 800.0f;

	UFUNCTION(BlueprintCallable)
	ASassGameManager* GetGameManager();
	
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void GameStart();
	virtual void GameStart_Implementation();
	virtual bool GameStart_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void LateStart();
	virtual void LateStart_Implementation();
	virtual bool LateStart_Validate();

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void HandleNewPlayer(ASassPlayerState* PlayerState);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASassGameManager* SassGameManager;
	
private:
};
