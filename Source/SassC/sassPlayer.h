// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "sassPlayer.generated.h"

UCLASS()
class SASSC_API AsassPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AsassPlayer();

	// Input
	//WASD Movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	//Camera Movement
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);
	//Sprint functions
	void SprintPressed();
	void SprintReleased();
	//Crouch functions
	void CrouchPressed();
	void CrouchReleased();
	//Jump function
	void JumpPressed();
	//Pause function
	void PausePressed();
	//Quit function
	void QuitGame();
	//test functions
	void testFunction();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSprint(bool isRunning, UCharacterMovementComponent *movementComponent);
	virtual void ServerSprint_Implementation(bool isRunning, UCharacterMovementComponent *movementComponent);
	virtual bool ServerSprint_Validate(bool isRunning, UCharacterMovementComponent *movementComponent);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCrouch(bool isCrouching, UCharacterMovementComponent *movementComponent);
	virtual void ServerCrouch_Implementation(bool isCrouching, UCharacterMovementComponent *movementComponent);
	virtual bool ServerCrouch_Validate(bool isCrouching, UCharacterMovementComponent *movementComponent);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ChangePauseWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UUserWidget> PauseWidgetClass;

private:
	bool InvertPitch = false;
	bool InvertYaw = false;
	bool IsCrouchPressed = false;
	bool IsSprintPressed = false;
	bool IsPaused = false;
	float SprintSpeed = 800.0f;
	float WalkSpeed = 400.0f;
	float CrouchSpeed = 200.0f;
	float CrouchingEyeHeight = 40.0f;
	float StandingEyeHeight = 80.0f;
	UUserWidget* PauseWidget;
	FInputModeGameAndUI GameAndUI;
	FInputModeGameOnly GameOnly;
};
