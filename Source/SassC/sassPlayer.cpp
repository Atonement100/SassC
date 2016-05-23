// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
#include "sassPauseMenu.h"
#include "sassPlayerController.h"
#include "sassPlayerState.h"
#include "unitBase.h"
#include "Kismet/KismetStringLibrary.h" //necessary only for debugging


AsassPlayer::AsassPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AsassPlayer::BeginPlay()
{
	Super::BeginPlay();
	PlayerControllerPtr = UGameplayStatics::GetPlayerController(this, 0);
	CleanupHUD();
	CreatePregameHUD();

}

void AsassPlayer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
}

void AsassPlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	//WASD Movement
	InputComponent->BindAxis("MoveForward", this, &AsassPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AsassPlayer::MoveRight);
	//Camera Movement
	InputComponent->BindAxis("PitchCamera", this, &AsassPlayer::PitchCamera);
	InputComponent->BindAxis("YawCamera", this, &AsassPlayer::YawCamera);
	//Sprint functions
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AsassPlayer::SprintPressed);
	InputComponent->BindAction("Sprint", IE_Released, this, &AsassPlayer::SprintReleased);
	//Crouch functions
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AsassPlayer::CrouchPressed);
	InputComponent->BindAction("Crouch", IE_Released, this, &AsassPlayer::CrouchReleased);
	//Jump function
	InputComponent->BindAction("Jump", IE_Pressed, this, &AsassPlayer::JumpPressed);
	//Pause function
	InputComponent->BindAction("Pause", IE_Pressed, this, &AsassPlayer::PausePressed);
	//Quit function
	InputComponent->BindAction("ForceQuit", IE_Pressed, this, &AsassPlayer::QuitGame);
	//UnitMenu function
	InputComponent->BindAction("UnitMenu", IE_Pressed, this, &AsassPlayer::UnitMenuPressed);
	//test functions
	InputComponent->BindAction("Test", IE_Pressed, this, &AsassPlayer::testFunction);
}

void AsassPlayer::UnitMenuPressed() {
	if (!IsUnitMenuOpen) {
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->bShowMouseCursor = true;
			PlayerControllerPtr->SetInputMode(GameAndUI);
		}

		IsUnitMenuOpen = true;
	}
	else {
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->bShowMouseCursor = false;
			PlayerControllerPtr->SetInputMode(GameOnly);
		}
		IsUnitMenuOpen = false;
	}
}

void AsassPlayer::testFunction() {
	bool ownedLocally = IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, UKismetStringLibrary::Conv_BoolToString(ownedLocally));
	
}

void AsassPlayer::CreatePregameHUD() {
	if (PlayerControllerPtr->IsLocalController() && PregameWidget == nullptr) {
		PregameWidget = CreateWidget<UUserWidget>(PlayerControllerPtr, PregameWidgetClass);
		if (PregameWidget != nullptr) PregameWidget->AddToViewport(); 
	}
}

void AsassPlayer::RemoveAllWidgets() {
	for (TObjectIterator<UUserWidget> WidgetIter; WidgetIter; ++WidgetIter) {
		UUserWidget* CurrentWidget = *WidgetIter;
		if (!CurrentWidget->GetWorld()) continue;
		else CurrentWidget->RemoveFromParent();
	}
}

void AsassPlayer::CreateGameHUD() {
	if (PlayerControllerPtr->IsLocalController() && GameWidget == nullptr) {
		RemoveAllWidgets();
		GameWidget = CreateWidget<UUserWidget>(PlayerControllerPtr, GameWidgetClass);
		if (GameWidget != nullptr) GameWidget->AddToViewport();
		PlayerControllerPtr->SetInputMode(GameOnly);
		PlayerControllerPtr->bShowMouseCursor = false;
	}
}

void AsassPlayer::GetAllPlayerColors() {
	TArray<AActor*> FoundControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerControllerClass, FoundControllers);
	for (AActor* Controller : FoundControllers) {
		
	}
}

void AsassPlayer::QuitGame() {
	FGenericPlatformMisc::RequestExit(false);
}

#pragma region Pause functions
void AsassPlayer::PausePressed() {
	if (!IsPaused) {
		if (PauseWidgetClass != nullptr) PauseWidget = CreateWidget<UsassPauseMenu>(PlayerControllerPtr, PauseWidgetClass);
		if (PauseWidget != nullptr) PauseWidget->AddToViewport();
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->SetInputMode(GameAndUI);
			PlayerControllerPtr->SetIgnoreMoveInput(true);
			PlayerControllerPtr->bShowMouseCursor = true;
		}
		IsPaused = true;
	}
	else {
		if (PauseWidget != nullptr) {
			PauseWidget->RemoveFromViewport();
			PauseWidget = nullptr;
		}
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->SetInputMode(GameOnly);
			PlayerControllerPtr->SetIgnoreMoveInput(false);
			PlayerControllerPtr->bShowMouseCursor = false;
		}
		IsPaused = false;
	}
}

void AsassPlayer::ChangePauseWidget(TSubclassOf<UUserWidget> NewWidgetClass) {
	if (PauseWidget != nullptr) {
		PauseWidget->RemoveFromViewport();
		PauseWidget = nullptr;
	}
	if (NewWidgetClass != nullptr) {
		PauseWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (PauseWidget != nullptr) PauseWidget->AddToViewport();
	}
}
#pragma endregion

#pragma region Jump function
void AsassPlayer::JumpPressed() {
	Jump();
}
#pragma endregion

#pragma region Crouch functions
void AsassPlayer::CrouchPressed() {
	IsCrouchPressed = true;
	IsSprintPressed = false;
	BaseEyeHeight = CrouchingEyeHeight;
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	ServerCrouch(IsCrouchPressed, GetCharacterMovement());
}

void AsassPlayer::CrouchReleased() {
	IsCrouchPressed = false;
	BaseEyeHeight = StandingEyeHeight;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ServerCrouch(IsCrouchPressed, GetCharacterMovement());
}

void AsassPlayer::ServerCrouch_Implementation(bool isCrouching, UCharacterMovementComponent *movementComponent) {
	if (isCrouching) { movementComponent->MaxWalkSpeed = CrouchSpeed; }
	else { movementComponent->MaxWalkSpeed = WalkSpeed; }
}

bool AsassPlayer::ServerCrouch_Validate(bool isCrouching, UCharacterMovementComponent *movementComponent) {
	return true;
}

#pragma endregion

#pragma region Sprint functions
void AsassPlayer::SprintPressed() {
	IsSprintPressed = true;
	IsCrouchPressed = false;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	ServerSprint(IsSprintPressed, GetCharacterMovement());
}

void AsassPlayer::SprintReleased() {
	IsSprintPressed = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ServerSprint(IsSprintPressed, GetCharacterMovement());
}

void AsassPlayer::ServerSprint_Implementation(bool isRunning, UCharacterMovementComponent *movementComponent) {
	if (isRunning) movementComponent->MaxWalkSpeed = SprintSpeed;
	else movementComponent->MaxWalkSpeed = WalkSpeed;
}

bool AsassPlayer::ServerSprint_Validate(bool isRunning, UCharacterMovementComponent *movementComponent) {
	return true;
}
#pragma endregion

#pragma region WASD Movement
void AsassPlayer::MoveForward(float AxisValue) {
	if (Controller != NULL && AxisValue != 0.0f) {
		FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void AsassPlayer::MoveRight(float AxisValue) {
	if (Controller != NULL && AxisValue != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}
#pragma endregion

#pragma region Camera Movement
void AsassPlayer::PitchCamera(float AxisValue) {
	if (Controller != NULL && AxisValue != 0.0f) {
		if (InvertPitch) {
			AddControllerPitchInput(-AxisValue);
		}
		else {
			AddControllerPitchInput(AxisValue);
		}	
	}
}

void AsassPlayer::YawCamera(float AxisValue) {
	if (Controller != NULL && AxisValue != 0.0f) {
		if (InvertYaw) {
			AddControllerYawInput(-AxisValue);
		}
		else {
			AddControllerYawInput(AxisValue);
		}
	}
}
#pragma endregion

UUserWidget* AsassPlayer::GetGameWidget() {
	return GameWidget;
}

AActor* AsassPlayer::GetSelectionSphereHolder() {
	return SelectionSphereHolder;
}

#pragma region Unit Interaction
void AsassPlayer::TurnOffAllSelectionCircles()
{
	for (AActor* UnitIt : (((AsassPlayerState*)PlayerState)->ControlledBuildings)) {
		AunitBase* Unit = (AunitBase*)UnitIt;
		Unit->SetDecalVisibility(Unit->SelectionCircleDecal, false);
	}
}

void AsassPlayer::CreateSelectedUnitsArray(TArray<FHitResult> Hits)
{
	SelectedUnits.Empty();
	AsassPlayerState* TempPlayerState = ((AsassPlayerState*)PlayerState);

	for (FHitResult Hit : Hits) {
		AunitBase* Unit = (AunitBase*)(Hit.GetActor());
		//TODO:
		//When ownership of pawns is set up, change the second half of this statement
		//to check for Unit->owner = calling player... much more efficient.
		if (Unit != nullptr && TempPlayerState->ControlledBuildings.Contains(Unit)) {
			SelectedUnits.Add(Unit);
			Unit->SetDecalVisibility(Unit->SelectionCircleDecal, true);
		}
	}

	TempPlayerState->SelectedUnits = SelectedUnits;
}
#pragma endregion