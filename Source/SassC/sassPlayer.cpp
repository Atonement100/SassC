// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassPlayer.h"
#include "sassPauseMenu.h"
#include "sassPlayerController.h"
#include "sassPlayerState.h"
#include "sassGameState.h"
#include "unitBase.h"
#include "buildingBase.h"
#include "SassCStaticLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h" //necessary only for debugging

#define MAX_BLDG_CORNER_DIFFERENCE 6.0f


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
	GetWorld();
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
	//Mouse functions
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &AsassPlayer::LeftClickPressed);
	InputComponent->BindAction("LeftClick", IE_Released, this, &AsassPlayer::LeftClickReleased);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &AsassPlayer::RightClickPressed);
	InputComponent->BindAction("RightClick", IE_Released, this, &AsassPlayer::RightClickReleased);
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
	//test functions, default L
	InputComponent->BindAction("Test", IE_Pressed, this, &AsassPlayer::testFunction);
}

void AsassPlayer::testFunction() {

}

void AsassPlayer::LeftClickPressed() {
	IsLeftMouseDown = true;
	if (IsUnitMenuOpen){
		if (PlayerControllerPtr == nullptr) return;
		FHitResult Hit;
		TArray<FVector> LocationsToCheck;

		PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit); // Assign Hit
		AbuildingBase* LocalBuildingRef = Cast<AbuildingBase>(LocalObjectSpawn);
		if (LocalBuildingRef != nullptr) { LocationsToCheck = LocalBuildingRef->CornerLocations; }
		//may need to do this for unit as well

		ServerSpawnBuilding(Cast<AsassPlayerController>(PlayerControllerPtr), SelectedSpawnableClass, Hit, FVector::ZeroVector, LocationsToCheck);
	}
}

void AsassPlayer::LeftClickReleased() {
	IsLeftMouseDown = false;
}

void AsassPlayer::RightClickPressed() {

}

void AsassPlayer::RightClickReleased() {

}

#pragma region HUD functions
void AsassPlayer::UnitMenuPressed() {
	if (!IsUnitMenuOpen) {
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->bShowMouseCursor = true;
			PlayerControllerPtr->SetInputMode(GameAndUI);
		}

		//IsUnitMenuOpen = true;
	}
	else {
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->bShowMouseCursor = false;
			PlayerControllerPtr->SetInputMode(GameOnly);
		}
		//IsUnitMenuOpen = false;
	}
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
		SetSassHUDRef();
	}
}

UUserWidget* AsassPlayer::GetGameWidget() {
	return GameWidget;
}

#pragma endregion

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

#pragma region Unit Interaction
void AsassPlayer::TurnOffAllSelectionCircles()
{
	if (PlayerState == nullptr) { return; }
	for (AActor* UnitIt : (((AsassPlayerState*)PlayerState)->ControlledBuildings)) {
		AunitBase* Unit = Cast<AunitBase>(UnitIt);
		if (Unit != nullptr) { Unit->SetDecalVisibility(Unit->SelectionCircleDecal, false); }
	}
}

void AsassPlayer::CreateSelectedUnitsArray(TArray<FHitResult> Hits)
{
	SelectedUnits.Empty();
	AsassPlayerState* TempPlayerState = ((AsassPlayerState*)PlayerState);

	if (TempPlayerState == nullptr) { return; } //Shouldn't happen

	for (FHitResult Hit : Hits) {
		AunitBase* Unit = Cast<AunitBase>(Hit.GetActor());
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

AActor* AsassPlayer::GetSelectionSphereHolder() {
	return SelectionSphereHolder;
}

#pragma endregion

void AsassPlayer::ServerSpawnBuilding_Implementation(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, const FVector &HalfHeight, const TArray<FVector> &Midpoints)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, UKismetStringLibrary::Conv_VectorToString(Hit.Location));
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	const FVector Location = Hit.Location + FVector(0,0,50);
	const FRotator Rotation = this->GetActorRotation();
	GetWorld()->SpawnActor(SelectedSpawnableClass, &Location, &Rotation, SpawnParams);
}

bool AsassPlayer::ServerSpawnBuilding_Validate(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, const FVector &HalfHeight, const TArray<FVector> &Midpoints)
{
	return true;
}

void AsassPlayer::ColorPlayer_Implementation(FLinearColor PlayerColor)
{
	if (DynamicPlayerMaterial == nullptr || GetMesh() == nullptr) return;
	DynamicPlayerMaterial->SetVectorParameterValue(ColorParameterName, PlayerColor);
	GetMesh()->SetMaterial(0, DynamicPlayerMaterial);
}

bool AsassPlayer::ColorPlayer_Validate(FLinearColor PlayerColor)
{
	return true;
}

bool AsassPlayer::CheckBldgCorners(TArray<FVector> ExtraLocs, FVector Center)
{
	FHitResult Hit;
	TArray<float> TraceHeights;
	for (FVector Loc : ExtraLocs) {	
		if (USassCStaticLibrary::Trace(GetWorld(), this, Center + Loc, Center + Loc - FVector(0, 0, 35), Hit, WorldStatic)) {
			TraceHeights.Add(Hit.Location.Z);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "SassPlayer CheckBldgCorners: TRACE MADE NO CONTACT");
			return true;
		}
	}
	float first = TraceHeights[0];
	for (float Height : TraceHeights) {
		if (FMath::Abs(Height - first) > MAX_BLDG_CORNER_DIFFERENCE) {
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "SassPlayer CheckBldgCorners: HEIGHTS NOT EQUAL");
			return true;
		}
	}

	return false;
}

void AsassPlayer::SetSassHUDRef_Implementation()
{
}
