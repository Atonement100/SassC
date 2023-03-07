// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/PlayerController.h"
#include "Gamemode/Sassilization/SassGameManager.h"
#include "SassPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API ASassPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASassPlayerController();

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void UnitMenuPressed();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void StoreGameWidget(const UUserWidget* GameWidget);
	
	virtual void BeginPlay() override;
	virtual void BeginPlayingState() override;
	
	UFUNCTION(BlueprintCallable)
	void CreatePregameHUD();
	UFUNCTION(BlueprintCallable)
	void CreateGameHUD();
	UFUNCTION(BlueprintCallable) 
	void UpdateHUD(TSubclassOf<UUserWidget> WidgetToCreate);
	UFUNCTION(BlueprintCallable)
	bool IsSpawnableRequestValid();
	UFUNCTION(BlueprintCallable)
	bool RequestSpawnable();
	UFUNCTION(BlueprintCallable)
	bool RequestBuildingSpawn();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Gamemode")
	ASassGameManager* SassGameManager;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> PregameWidgetClass;
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> GameWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	UUserWidget* ActiveWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	bool bIsUnitMenuOpen;
};
