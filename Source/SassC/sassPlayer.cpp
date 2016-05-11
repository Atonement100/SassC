// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
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
	InputComponent->BindAxis("MoveForward", this, &AsassPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AsassPlayer::MoveRight);
	InputComponent->BindAxis("PitchCamera", this, &AsassPlayer::PitchCamera);
	InputComponent->BindAxis("YawCamera", this, &AsassPlayer::YawCamera);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AsassPlayer::SprintPressed);
	InputComponent->BindAction("Sprint", IE_Released, this, &AsassPlayer::SprintReleased);
	InputComponent->BindAction("Test", IE_Pressed, this, &AsassPlayer::testFunction);
}

void AsassPlayer::testFunction() {
	bool ownedLocally = IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, UKismetStringLibrary::Conv_BoolToString(ownedLocally));
	
}

void AsassPlayer::SprintPressed() {
	IsSprintPressed = true;
	IsCrouchPressed = false;
	ServerSprint(IsSprintPressed, GetCharacterMovement());
}

void AsassPlayer::SprintReleased() {
	IsSprintPressed = false;
	ServerSprint(IsSprintPressed, GetCharacterMovement());
}

void AsassPlayer::ServerSprint_Implementation(bool isRunning, UCharacterMovementComponent *movementComponent) {
	if (isRunning) movementComponent->MaxWalkSpeed = SprintSpeed;
	else movementComponent->MaxWalkSpeed = WalkSpeed;
}

bool AsassPlayer::ServerSprint_Validate(bool isRunning, UCharacterMovementComponent *movementComponent) {
	return true;
}

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
