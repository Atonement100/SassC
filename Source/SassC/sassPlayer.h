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
class Awall;
class AsassPlayerController;

UENUM()
enum class ETypeOfOrder : uint8 {
	ORDER_UNIT			UMETA(DisplayName = "Unit"),
	ORDER_BUILDING		UMETA(DisplayName = "Building"),
	ORDER_WORLD			UMETA(DisplayName = "World")
};

UENUM()
enum class ETypeOfSpawnable : uint8 {
	BUILDING_CITY		UMETA(DisplayName = "City"),
	BUILDING_WORKSHOP	UMETA(DisplayName = "Workshop"),
	BUILDING_TOWER		UMETA(DisplayName = "Tower"),
	BUILDING_WALL		UMETA(DisplayName = "Wall"),
	BUILDING_GATE		UMETA(DisplayName = "Gate"),
	BUILDING_SHIELDMONO UMETA(DisplayName = "Shield Monolith"),
	BUILDING_SHRINE		UMETA(DisplayName = "Shrine"),
	UNIT_SOLDIER		UMETA(DisplayName = "Soldier"),
	UNIT_ARCHER			UMETA(DisplayName = "Archer"),
	UNIT_SCALLYWAG		UMETA(DisplayName = "Scallywag"),
	UNIT_BALLISTA		UMETA(DisplayName = "Ballista"),
	UNIT_CATAPULT		UMETA(DisplayName = "Catapult")
};

UCLASS()
class SASSC_API AsassPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AsassPlayer();

	// Input
	/* Movement and client-state functions */
	/*Handles forward and backwards translation*/
	void MoveForward(float AxisValue);
	/*Handles side-to-side translation*/
	void MoveRight(float AxisValue);
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);
	void SprintPressed();
	void SprintReleased();
	void CrouchPressed();
	void CrouchReleased();
	void JumpPressed();
	/*Enables selection sphere gate for tick or handles building spawns, depending on if Unit Menu is open*/
	void LeftClickPressed();
	/*Disables selection sphere*/
	void LeftClickReleased();
	/*Handles unit dispatching*/
	void RightClickPressed();
	void RightClickReleased();
	/*Toggles unit menu*/
	void UnitMenuPressed();
	/*Toggles escape/pause menu */
	UFUNCTION(BlueprintCallable, Category = "Sass Player")
	void PausePressed();
	//@TODO: Bring up confirmation menu for f10 exiting if shipping with force quit
	/*Force quits game (F10)*/
	void QuitGame();

	UFUNCTION(BlueprintCallable, Category = "Sass Player")
	void UpdateSelectedSpawnableClass(UClass* NewClass);

	//test functions
	void testFunction();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/*Dispatches units on server*/
	UFUNCTION(Reliable, Server, WithValidation)
	void CommandUnits(const TArray<AunitBase*> &SelectedUnits, FHitResult RaycastHit, ETypeOfOrder OrderType);
	virtual void CommandUnits_Implementation(const TArray<AunitBase*> &SelectedUnits, FHitResult RaycastHit, ETypeOfOrder OrderType);
	virtual bool CommandUnits_Validate(const TArray<AunitBase*> &SelectedUnits, FHitResult RaycastHit, ETypeOfOrder OrderType);

	/*Registers sprinting movement with server*/
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSprint(bool isRunning, UCharacterMovementComponent *movementComponent);
	virtual void ServerSprint_Implementation(bool isRunning, UCharacterMovementComponent *movementComponent);
	virtual bool ServerSprint_Validate(bool isRunning, UCharacterMovementComponent *movementComponent);

	/*Registers crouching movement with server*/
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCrouch(bool isCrouching, UCharacterMovementComponent *movementComponent);
	virtual void ServerCrouch_Implementation(bool isCrouching, UCharacterMovementComponent *movementComponent);
	virtual bool ServerCrouch_Validate(bool isCrouching, UCharacterMovementComponent *movementComponent);

	/*Request from player to spawn building on server*/
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSpawnBuilding(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, FRotator Rotator, const FVector &HalfHeight, const TArray<FVector> &Midpoints, const FVector &TraceSize, int32 PlayerID, const TArray<AActor*> &ActorsToIgnore = TArray<AActor*>());
	virtual void ServerSpawnBuilding_Implementation(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, FRotator Rotator, const FVector &HalfHeight, const TArray<FVector> &Midpoints, const FVector &TraceSize, int32 PlayerID, const TArray<AActor*> &ActorsToIgnore = TArray<AActor*>());
	virtual bool ServerSpawnBuilding_Validate(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, FRotator Rotator, const FVector &HalfHeight, const TArray<FVector> &Midpoints, const FVector &TraceSize, int32 PlayerID, const TArray<AActor*> &ActorsToIgnore = TArray<AActor*>());

	/*Gets playercolor associated with a player when they spawn and updates their playermodel*/
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ColorPlayer(FLinearColor PlayerColor);
	virtual void ColorPlayer_Implementation(FLinearColor PlayerColor);
	virtual bool ColorPlayer_Validate(FLinearColor PlayerColor);

	UFUNCTION(Reliable, Client, WithValidation)
	void LateStart(APlayerController* NewPlayer);
	virtual void LateStart_Implementation(APlayerController* NewPlayer);
	virtual bool LateStart_Validate(APlayerController* NewPlayer);

	/*Handles toggling of pause menu*/
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ChangePauseWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	/*@DEPRECATED (Blueprint Implemented) Removes all widgets from HUD*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
	void CleanupHUD();

	/*Creates HUD used for pregame, displays time to start*/
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void CreatePregameHUD();

	/*Creates main game HUD, including unit menu*/
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void CreateGameHUD();

	/*Calls for all player models to be colored for clients when round starts (between pre-game and game start, when all clients should have connected)*/
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void GetAllPlayerColors();

	/*Removes all widgets from HUD*/
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RemoveAllWidgets();

	/*@DEPRECATED Returns GameWidget (which has been made public)*/
	UFUNCTION(BlueprintCallable, Category = "HUD")
	UUserWidget* GetGameWidget();

	/*Returns selection sphere reference*/
	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	AActor* GetSelectionSphereHolder();

	/*Turns off all unit selection decals for owned units (which are all that can be seen by each client)*/
	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	void TurnOffAllSelectionCircles();

	/*Creates array used for dispatching units*/
	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	void CreateSelectedUnitsArray(TArray<FHitResult> Hits, int32 PlayerID, bool AddToSelection);

	/*Returns true if there is an issue with the spawn corners*/
	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	bool CheckBldgCorners(TArray<FVector> ExtraLocs, FVector Center, FRotator Rotator, int32 PlayerID, bool isCity);

	/*Returns true if there is an issue with the spawn location*/
	UFUNCTION(BlueprintCallable, Category = "Spawnables")
	bool CheckUnitLocation(FVector Center, FRotator Rotator, int32 PlayerID);

	/*Blueprint-set class of the pause widget*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	/*Blueprint-set class of the pre-game widget*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UUserWidget> PregameWidgetClass;

	/*Blueprint-set class of the game widget*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UUserWidget> GameWidgetClass;

	/*Blueprint-set class of the player controller*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<AActor> PlayerControllerClass;

	/*Blueprint-handled player material (used for player coloring)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	UMaterialInstanceDynamic* DynamicPlayerMaterial;

	/*Spawnable the player currently as selected, for verification and tick local-spawning*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	UClass* SelectedSpawnableClass;

	/*Reference to spawned game widget*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sass Player")
	UUserWidget* GameWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	AActor* SelectionSphereHolder;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSpawnWall(Awall* NewWall, Awall* TargetWall, int32 PlayerID, FLinearColor PlayerColor);
	virtual void ServerSpawnWall_Implementation(Awall* NewWall, Awall* TargetWall, int32 PlayerID, FLinearColor PlayerColor);
	virtual bool ServerSpawnWall_Validate(Awall* NewWall, Awall* TargetWall, int32 PlayerID, FLinearColor PlayerColor);

protected:
	/*(Blueprint Native) Sets reference to game hud, so it can be used in code*/
	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void SetSassHUDRef();
	virtual void SetSassHUDRef_Implementation();
	/*(Blueprint Native) Opens unit menu, handled in blueprint for improved workflow and simplicity*/
	UFUNCTION(BlueprintNativeEVent, Category = "HUD")
	void OpenUnitMenu();
	virtual void OpenUnitMenu_Implementation();
	/*(Blueprint Native) Closes unit menu, handled in blueprint for improved workflow and simplicity*/
	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void CloseUnitMenu();
	virtual void CloseUnitMenu_Implementation();
	UFUNCTION(Unreliable, Client)
	void SpawnWallPreview(FVector Location, FRotator Rotation);
	virtual void SpawnWallPreview_Implementation(FVector Location, FRotator Rotation);

	UFUNCTION(Category = "Sass Player")
	bool ShouldIgnoreLookInput();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	AActor* LocalObjectSpawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sass Player")
	FName ColorParameterName = "PlayerColor";
	bool InvertPitch = false;
	bool InvertYaw = false;
	bool IsCrouchPressed = false;
	bool IsSprintPressed = false;
	bool IsPaused = false;
#pragma region Tick Variables
	bool ActorDestroyLatch = false;
	bool ActorSpawnLatch = true;
	bool SphereDestroyLatch = false;
	bool SphereSpawnLatch = true;
	bool ResetWallPreviewLatch = false;
	bool ResetGatePreviewLatch = true;
	bool TickRaycastLatch = true;
	bool IsBadSpawn = true;
	bool ResetLocalView = false;
	TArray<FHitResult> SphereTraceHits;
	TArray<Awall*> WallPreviewArray = TArray<Awall*>();
	TArray<Awall*> WallsBeingPreviewed = TArray<Awall*>();
	TArray<Awall*> TempGateWallsBeingTargeted = TArray<Awall*>();
	TArray<Awall*> TempGateWallsBeingPreviewed = TArray<Awall*>();
	TArray<AActor*> TempGateWalls = TArray<AActor*>();
	FRotator PreviewRotation = FRotator::ZeroRotator;
	FVector CurrentHit;
	FVector InitialHit;
	float SphereTraceRadius;
	UClass* LocalObjectClass;
#pragma endregion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	FVector CurrentHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	FVector CurrentTraceSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sass Player")
	bool IsUnitMenuOpen = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sass Player")
	bool IsLeftMouseDown = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sass Player")
	bool IsRightMouseDown = false;
	bool ShouldAddToSelection = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sass Player")
	UClass* SelectionSphereClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sass Player")
	UClass* WallSegmentClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sass Player")
	UClass* WallSegmentGhostClass;
	const float SprintSpeed = 800.0f;
	const float WalkSpeed = 400.0f;
	const float CrouchSpeed = 200.0f;
	const float CrouchingEyeHeight = 40.0f;
	const float StandingEyeHeight = 80.0f;
	UUserWidget* PauseWidget;
	UUserWidget* PregameWidget;
	APlayerController* PlayerControllerPtr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sass Player")
	TArray<AunitBase*> SelectedUnits;
	TArray<AActor*> WorldStaticObjects;
	/*Enum array of world dynamic and world static object types, for traces*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sass Player")
	TArray<TEnumAsByte<EObjectTypeQuery>> DynamicAndStaticObjectTypes;
	/*Enum array of world static object type, for traces*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sass Player")
	TArray<TEnumAsByte<EObjectTypeQuery>> StaticObjectTypes;
	/*Enum array of world dynamic object type, for traces*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sass Player")
	TArray<TEnumAsByte<EObjectTypeQuery>> DynamicObjectTypes;
	FCollisionObjectQueryParams WorldStatic = FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic); //const not allowed
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sass Player")
	ETypeOfSpawnable SelectedSpawnableType = ETypeOfSpawnable::BUILDING_CITY;

	const int SelectionSphereScaleMod = 100; // this should never ever be changed

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sass Player")
	UClass*	WallClass;
};
