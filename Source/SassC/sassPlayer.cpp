// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
#include "sassPauseMenu.h"
#include "sassPlayerController.h"
#include "Kismet/KismetStringLibrary.h" //necessary only for debugging


// Sets default values
AsassPlayer::AsassPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AsassPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AsassPlayer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
}

// Called to bind functionality to input
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
	//Pause functions
	InputComponent->BindAction("Pause", IE_Pressed, this, &AsassPlayer::PausePressed);
	//test functions
	InputComponent->BindAction("Test", IE_Pressed, this, &AsassPlayer::testFunction);
}

void AsassPlayer::testFunction() {
	bool ownedLocally = IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, UKismetStringLibrary::Conv_BoolToString(ownedLocally));
	
}

void AsassPlayer::PausePressed() {
	APlayerController* playerControllerPtr = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsPaused) {
		if (PauseWidgetClass != nullptr) PauseWidget = CreateWidget<UsassPauseMenu>(playerControllerPtr, PauseWidgetClass);
		if (PauseWidget != nullptr) PauseWidget->AddToViewport();
		if (playerControllerPtr != nullptr) {
			playerControllerPtr->SetInputMode(GameAndUI);
			playerControllerPtr->SetIgnoreMoveInput(true);
			playerControllerPtr->bShowMouseCursor = true;
		}
		IsPaused = true;
	}
	else {
		if (PauseWidget != nullptr) {
			PauseWidget->RemoveFromViewport();
			PauseWidget = nullptr;
		}
		if (playerControllerPtr != nullptr) {
			playerControllerPtr->SetInputMode(GameOnly);
			playerControllerPtr->SetIgnoreMoveInput(false);
			playerControllerPtr->bShowMouseCursor = false;
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
