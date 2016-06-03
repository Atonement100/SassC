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

class AunitBase;
class AsassPlayerController;

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
	//Mouse functions
	void LeftClickPressed();
	void LeftClickReleased();
	void RightClickPressed();
	void RightClickReleased();
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
	//UnitMenu function
	void UnitMenuPressed();
	//test functions
	void testFunction();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSprint(bool isRunning, UCharacterMovementComponent *movementComponent);
	virtual void ServerSprint_Implementation(bool isRunning, UCharacterMovementComponent *movementComponent);
	virtual bool ServerSprint_Validate(bool isRunning, UCharacterMovementComponent *movementComponent);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCrouch(bool isCrouching, UCharacterMovementComponent *movementComponent);
	virtual void ServerCrouch_Implementation(bool isCrouching, UCharacterMovementComponent *movementComponent);
	virtual bool ServerCrouch_Validate(bool isCrouching, UCharacterMovementComponent *movementComponent);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSpawnBuilding(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, const FVector &HalfHeight, const TArray<FVector> &Midpoints);
	virtual void ServerSpawnBuilding_Implementation(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, const FVector &HalfHeight, const TArray<FVector> &Midpoints);
	virtual bool ServerSpawnBuilding_Validate(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, const FVector &HalfHeight, const TArray<FVector> &Midpoints);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorPlayer(FLinearColor PlayerColor);
	virtual void ColorPlayer_Implementation(FLinearColor PlayerColor);
	virtual bool ColorPlayer_Validate(FLinearColor PlayerColor);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ChangePauseWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
	void CleanupHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void CreatePregameHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void CreateGameHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void GetAllPlayerColors();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RemoveAllWidgets();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	UUserWidget* GetGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	AActor* GetSelectionSphereHolder();

	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	void TurnOffAllSelectionCircles();

	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	void CreateSelectedUnitsArray(TArray<FHitResult> Hits);

	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	bool CheckBldgCorners(TArray<FVector> ExtraLocs, FVector Center);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UUserWidget> PregameWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UUserWidget> GameWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<AActor> PlayerControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	UMaterialInstanceDynamic* DynamicPlayerMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	UClass* SelectedSpawnableClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sass Player")
		UUserWidget* GameWidget;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void SetSassHUDRef();
	virtual void SetSassHUDRef_Implementation();
	UFUNCTION(BlueprintNativeEVent, Category = "HUD")
	void OpenUnitMenu();
	virtual void OpenUnitMenu_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void CloseUnitMenu();
	virtual void CloseUnitMenu_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	AActor* LocalObjectSpawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sass Player")
	FName ColorParameterName = "PlayerColor";
	bool InvertPitch = false;
	bool InvertYaw = false;
	bool IsCrouchPressed = false;
	bool IsSprintPressed = false;
	bool IsPaused = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sass Player")
	bool IsUnitMenuOpen = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sass Player")
	bool IsLeftMouseDown = false;
	const float SprintSpeed = 800.0f;
	const float WalkSpeed = 400.0f;
	const float CrouchSpeed = 200.0f;
	const float CrouchingEyeHeight = 40.0f;
	const float StandingEyeHeight = 80.0f;
	UUserWidget* PauseWidget;
	UUserWidget* PregameWidget;
	FInputModeGameAndUI GameAndUI;
	FInputModeGameOnly GameOnly;
	APlayerController* PlayerControllerPtr;
	AActor* SelectionSphereHolder;
	TArray<AunitBase*> SelectedUnits;
	TArray<AActor*> WorldStaticObjects;
	FCollisionObjectQueryParams WorldStatic = FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic); //const not allowed
};
