// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SassPlayerController.h"

#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Gamemode/Sassilization/SassGameManager.h"
#include "Gamemode/Sassilization/SassGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/SassPlayer.h"
#include "UI/SassilizationHUD.h"

ASassPlayerController::ASassPlayerController()
{
}

void ASassPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	//UnitMenu function
	InputComponent->BindAction("UnitMenu", IE_Pressed, this, &ASassPlayerController::UnitMenuPressed);
}

void ASassPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASassPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();
	UE_LOG(Sassilization, Display, TEXT("Begin Playing State Called"))

	ASassGameState* GameState = GetWorld()->GetGameState<ASassGameState>();
	this->SassGameManager = GameState->GetGameManager();

	if (IsLocalController())
	{
		switch (GameState->GetGameStatus())
		{
		case ESassGameStatus::PreGame:
			CreatePregameHUD();
			break;
		case ESassGameStatus::GameActive:
			CreateGameHUD();
			break;
		default:
			UE_LOG(Sassilization, Warning, TEXT("Action not defined for Game Status %s"),
			       *UEnum::GetValueAsString(GameState->GetGameStatus()))
		}
	}
}

void ASassPlayerController::CreatePregameHUD()
{
	UpdateHUD(PregameWidgetClass);
}

void ASassPlayerController::CreateGameHUD()
{
	UpdateHUD(GameWidgetClass);
}

void ASassPlayerController::UpdateHUD(TSubclassOf<UUserWidget> WidgetToCreate)
{
	if (!WidgetToCreate)
	{
		UE_LOG(Sassilization, Warning, TEXT("Widget To Create is not configured"))
		return;
	}

	if (!this->IsLocalController())
	{
		UE_LOG(Sassilization, Warning, TEXT("UpdateHUD called on non-local controller"))
		return;
	}
	
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
	}
	
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(this, WidgetToCreate);

	if (!NewWidget)
	{
		UE_LOG(Sassilization, Warning, TEXT("Failed to create new hud of class %s"), *WidgetToCreate->GetName())
		return;
	}

	NewWidget->AddToViewport();
	NewWidget->SetVisibility(ESlateVisibility::Visible);
	ActiveWidget = NewWidget;
	this->SetInputMode(FInputModeGameOnly());
	this->bShowMouseCursor = false;

	if (NewWidget->IsA(USassilizationHUD::StaticClass()))
	{
		StoreGameWidget(NewWidget);
	}
}

bool ASassPlayerController::IsSpawnableRequestValid()
{
	return SassGameManager->IsValidBuildLocation();
}

bool ASassPlayerController::RequestSpawnable()
{
	return false;
}

bool ASassPlayerController::ServerRequestBuildingSpawn_Validate(ETypeOfEntity TypeOfEntity, FVector Location, FRotator Rotation)
{
	return true;
}

void ASassPlayerController::ServerRequestBuildingSpawn_Implementation(ETypeOfEntity TypeOfEntity, FVector Location, FRotator Rotation)
{
	SassGameManager->RequestBuildingSpawn(this, TypeOfEntity, Location, Rotation);
}

ASassGameManager* ASassPlayerController::GetSassGameManager() const
{
	return SassGameManager;
}

bool ASassPlayerController::IsUnitMenuOpen() const
{
	return bIsUnitMenuOpen;
}
