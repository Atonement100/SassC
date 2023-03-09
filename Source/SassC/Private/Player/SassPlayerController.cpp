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
	UE_LOG(LogTemp, Display, TEXT("Begin Playing State Called"))

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
			UE_LOG(LogTemp, Warning, TEXT("Action not defined for Game Status %s"),
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
		UE_LOG(LogTemp, Warning, TEXT("Widget To Create is not configured"))
		return;
	}

	if (!this->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateHUD called on non-local controller"))
		return;
	}
	
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
	}
	
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(this, WidgetToCreate);

	if (!NewWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create new hud of class %s"), *WidgetToCreate->GetName())
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

bool ASassPlayerController::RequestBuildingSpawn()
{
	ASassPlayer* SassPlayer = Cast<ASassPlayer>(this->GetPawn());
	TArray<TEnumAsByte<EObjectTypeQuery>> StaticObjectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
	StaticObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	const TArray<AActor*> RaycastIgnore;
	float BaseEyeHeight = SassPlayer->BaseEyeHeight;

	FHitResult InitRaycastHit;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(), SassPlayer->GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
		SassPlayer->GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
		UKismetMathLibrary::GetForwardVector(this->GetControlRotation()) * 10000.0f,
		StaticObjectTypes, true, RaycastIgnore, EDrawDebugTrace::ForOneFrame, InitRaycastHit, true);

	FVector InitialHit = InitRaycastHit.Location + 50.0f;

	return SassGameManager->RequestBuildingSpawn(this, GetPlayerState<ASassPlayerState>()->GetSelectedTypeOfEntity(),
	                                             InitialHit, FRotator(0, 0, 0));
}

ASassGameManager* ASassPlayerController::GetSassGameManager() const
{
	return SassGameManager;
}

bool ASassPlayerController::IsUnitMenuOpen() const
{
	return bIsUnitMenuOpen;
}
