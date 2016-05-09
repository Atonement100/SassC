// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);
	void SprintPressed();
	void SprintReleased();
	void testFunction();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	/*
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void Sprint(bool isRunning);
	virtual void Sprint_Implementation(bool isRunning);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprint(bool isRunning);
	virtual void ServerSprint_Implementation(bool isRunning);
	virtual bool ServerSprint_Validate(bool isRunning);
	*/


private:
	bool InvertPitch = false;
	bool InvertYaw = false;
	bool IsCrouchPressed = false;
	bool IsSprintPressed = false;
	float SprintSpeed = 800.0f;
	float WalkSpeed = 400.0f;
};
