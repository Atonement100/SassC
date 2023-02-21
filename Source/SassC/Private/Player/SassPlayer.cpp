// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/UnitController.h"
#include "Buildings/BuildingBase.h"
#include "Buildings/City.h"
#include "Buildings/Gate.h"
#include "Buildings/ShieldMonolith.h"
#include "Buildings/Tower.h"
#include "Buildings/Wall.h"
#include "Buildings/WallSegment.h"
#include "Buildings/Workshop.h"
#include "Core/SassCStaticLibrary.h"
#include "Gamemode/Sassilization/SassGameState.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h" //necessary only for debugging
#include "Kismet/KismetSystemLibrary.h"
#include "ParticleDefinitions.h"
#include "Player/SassPlayer.h"
#include "Player/SassPlayerController.h"
#include "Player/SelectionSphere.h"
#include "UI/SassPauseMenu.h"
#include "Units/UnitBase.h"
#include "UnrealNetwork.h"

#define MAX_BLDG_CORNER_DIFFERENCE 6.0f


ASassPlayer::ASassPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	TempGateWalls.SetNumZeroed(2); // This is done to give TempGateWalls an exact size, so that we can reference [0..1]
	// Gate walls are potentially a very frequent check, so this aids in cutting down unnecessary repetitive checks
}

void ASassPlayer::BeginPlay()
{
	Super::BeginPlay();
	PlayerControllerPtr = UGameplayStatics::GetPlayerController(this, 0);
	CleanupHUD();

	ASassGameState* SassGameStateRef = Cast<ASassGameState>(GetWorld()->GetGameState());
	if (!SassGameStateRef->PreGameActive)
	{
		CreateGameHUD();
	}
	else
	{
		CreatePregameHUD();
	}
}

void ASassPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASassPlayer, SelectedSpawnableClass);
}

void ASassPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsUnitMenuOpen)
	{
		FHitResult CursorHit;

		//@TODO: Find a better solution for this. Perhaps custom constructor taking in bNoCollisionFail value.
		FActorSpawnParameters TempParams = FActorSpawnParameters();
		TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);

		if (LocalObjectSpawn != nullptr && !UKismetMathLibrary::EqualEqual_ClassClass(
			SelectedSpawnableClass, LocalObjectClass))
		{
			LocalObjectSpawn->Destroy();
			ActorSpawnLatch = true;
		}

		if (ActorSpawnLatch)
		{
			if (PlayerControllerPtr) PlayerControllerPtr->GetHitResultUnderCursorByChannel(
				UEngineTypes::ConvertToTraceType(ECC_Visibility), true, CursorHit);
			FTransform Transform = FTransform(PreviewRotation, CursorHit.Location, FVector(1));

			LocalObjectSpawn = GetWorld()->SpawnActor(SelectedSpawnableClass, &Transform, SpawnParams);
			if (LocalObjectSpawn != nullptr)
			{
				LocalObjectSpawn->SetOwner(PlayerControllerPtr); //I don't know if this line actually works, honestly
				LocalObjectSpawn->SetActorEnableCollision(false);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "SassPlayer Tick LocalObject didn't spawn");
			}
			LocalObjectClass = SelectedSpawnableClass;
			if (SelectionSphereHolder) SelectionSphereHolder->Destroy();
			SphereSpawnLatch = true;
			ActorDestroyLatch = true;
			ActorSpawnLatch = false;
		}

		if (PlayerControllerPtr != nullptr) PlayerControllerPtr->GetHitResultUnderCursorByChannel(
			UEngineTypes::ConvertToTraceType(ECC_Visibility), true, CursorHit);
		if (LocalObjectSpawn != nullptr)
		{
			if (!IsRightMouseDown)
			{
				LocalObjectSpawn->SetActorLocation(
					CursorHit.Location + ((SelectedSpawnableType < ETypeOfSpawnable::UNIT_SOLDIER)
						                      ? FVector::ZeroVector
						                      : CurrentHalfHeight));
			}
			else if (IsRightMouseDown)
			{
				FHitResult RotateHit;
				TArray<AActor*> RaycastIgnores;
				UKismetSystemLibrary::LineTraceSingle(
					GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
					GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
					UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation()) * 10000.0f,
					UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::None,
					RotateHit, true);
				PreviewRotation = FRotator(
					0, (RotateHit.Location - LocalObjectSpawn->GetActorLocation()).Rotation().Yaw, 0) - FRotator(
					0, 90, 0);
				LocalObjectSpawn->SetActorRotation(PreviewRotation);
			}
		}

		if (CursorHit.GetActor() && !CursorHit.GetActor()->IsA(ABuildingBase::StaticClass()))
		{
			//Trace hit something that isn't a building
			if (ResetLocalView)
			{
				LocalObjectSpawn->SetActorHiddenInGame(false);
				ResetLocalView = false;
			}
			if (CursorHit.Normal.Z > .990)
			{
				//@TODO: HalfHeight and TraceSize will only ever change when spawnable changes. Instead of checking this on tick, check it when spawnable is switched.
				FVector HalfHeight = CurrentHalfHeight, TraceSize = CurrentTraceSize;


				FHitResult BoxTraceHit;
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(LocalObjectSpawn);
				IsBadSpawn = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), CursorHit.Location + FVector(0, 0, 2),
				                                                  CursorHit.Location + 2 * CurrentHalfHeight,
				                                                  FVector(CurrentTraceSize.X, CurrentTraceSize.Y, 0),
				                                                  PreviewRotation,
				                                                  UEngineTypes::ConvertToTraceType(
					                                                  ECollisionChannel::ECC_GameTraceChannel2), true,
				                                                  ActorsToIgnore, EDrawDebugTrace::ForOneFrame,
				                                                  BoxTraceHit, true);
				if (ABuildingBase* BuildingCast = Cast<ABuildingBase>(LocalObjectSpawn))
				{
					IsBadSpawn = IsBadSpawn | CheckBldgCorners(BuildingCast->CornerLocations, CursorHit.Location,
					                                           PreviewRotation, GetPlayerState()->GetPlayerId(),
					                                           (Cast<ACity>(LocalObjectSpawn) ? true : false));
				}
				else if (AUnitBase* UnitCast = Cast<AUnitBase>(LocalObjectSpawn))
				{
					IsBadSpawn = IsBadSpawn | CheckUnitLocation(CursorHit.Location, PreviewRotation,
					                                            GetPlayerState()->GetPlayerId());
				}
			}
			else
			{
				IsBadSpawn = true;
				for (AWall* Wall : WallsBeingPreviewed)
				{
					if (Wall->TempConnection)
					{
						Wall->TempConnection->Destroy();
						Wall->TempConnection = nullptr;
					}
				}
				WallsBeingPreviewed.Empty();
			}

			FLinearColor NewColor = IsBadSpawn ? FLinearColor(.7f, 0.0f, .058f) : FLinearColor(.093f, .59f, .153f);

			if (ABuildingBase* BuildingCast = Cast<ABuildingBase>(LocalObjectSpawn))
			{
				BuildingCast->UpdateMaterial(NewColor, false);
			}
			else if (AUnitBase* UnitCast = Cast<AUnitBase>(LocalObjectSpawn)) { UnitCast->UpdateMaterial(NewColor); }

			if (AGate* GateCast = Cast<AGate>(LocalObjectSpawn))
			{
				ResetGatePreviewLatch = true;
				const FTransform LeftTransform = FTransform(FRotator::ZeroRotator,
				                                            LocalObjectSpawn->GetActorLocation() + (PreviewRotation +
					                                            FRotator(0, 90, 0)).Vector() * 50);
				const FTransform RightTransform = FTransform(FRotator::ZeroRotator,
				                                             LocalObjectSpawn->GetActorLocation() + (PreviewRotation +
					                                             FRotator(0, 90, 0)).Vector() * -50);

				if (TempGateWalls[0] != nullptr) TempGateWalls[0]->SetActorTransform(LeftTransform);
				else if (!TempGateWalls[0])
				{
					TempGateWalls[0] = GetWorld()->SpawnActor(WallClass, &LeftTransform, SpawnParams);
					Cast<AWall>(TempGateWalls[0])->GetMesh()->SetCollisionResponseToChannel(
						ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
				}
				if (TempGateWalls[1] != nullptr) TempGateWalls[1]->SetActorTransform(RightTransform);
				else if (!TempGateWalls[1])
				{
					TempGateWalls[1] = GetWorld()->SpawnActor(WallClass, &RightTransform, SpawnParams);
					Cast<AWall>(TempGateWalls[1])->GetMesh()->SetCollisionResponseToChannel(
						ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
				}

				TempGateWallsBeingPreviewed.Empty();
				for (AActor* Wall : TempGateWalls)
				{
					if (!Wall) continue;
					AWall* WallInstance = Cast<AWall>(Wall);
					AWall* TargetWall = WallInstance->GetClosestWallTowerInRange(100.0f, TempGateWalls);
					if (!TargetWall)
					{
						if (WallInstance->TempConnection)
						{
							WallInstance->TempConnection->Destroy();
							WallInstance->TempConnection = nullptr;
						}
						continue;
					}
					TempGateWallsBeingTargeted.Add(TargetWall);
					FVector Displacement = TargetWall->GetActorLocation() - Wall->GetActorLocation();
					FVector UnitDirection = Displacement / Displacement.Size();
					FHitResult Hit;
					if (TargetWall->OwningPlayerID == GetPlayerState()->GetPlayerId() && !
						UKismetSystemLibrary::BoxTraceSingle(
							GetWorld(),
							WallInstance->GetActorLocation() + FVector(UnitDirection.X * 24, UnitDirection.Y * 24, 21),
							TargetWall->GetActorLocation() + FVector(UnitDirection.X * -24, UnitDirection.Y * -24, 21),
							FVector(9.5f, 4.0f, 15.0f), Displacement.Rotation(),
							UEngineTypes::ConvertToTraceType(ECC_Visibility), true, TempGateWalls,
							EDrawDebugTrace::ForOneFrame, Hit, true))
					{
						if (!WallInstance->TempConnection)
						{
							//create new
							FTransform Transform = FTransform(Displacement.Rotation(),
							                                  WallInstance->GetActorLocation() + (Displacement / 2),
							                                  FVector(Displacement.Size() / 19, 1, 1));
							WallInstance->TempConnection = GetWorld()->SpawnActor(
								WallSegmentGhostClass, &Transform, SpawnParams);
						}
						else if (WallInstance->TempConnection)
						{
							//update existing
							FVector Displacement = TargetWall->GetActorLocation() - WallInstance->GetActorLocation();
							FTransform Transform = FTransform(Displacement.Rotation(),
							                                  WallInstance->GetActorLocation() + (Displacement / 2),
							                                  FVector(Displacement.Size() / 19, 1, 1));
							WallInstance->TempConnection->SetActorTransform(Transform);
						}
					}
					else
					{
						if (WallInstance->TempConnection)
						{
							WallInstance->TempConnection->Destroy();
							WallInstance->TempConnection = nullptr;
						}
					}
				}
			}
			else if (ResetGatePreviewLatch)
			{
				ResetGatePreviewLatch = false;
				for (int Index = 0; Index < TempGateWalls.Num(); Index++)
				{
					if (!TempGateWalls[Index]) continue;
					if (AWall* WallRefCast = Cast<AWall>(TempGateWalls[Index]))
					{
						if (WallRefCast->TempConnection)
						{
							WallRefCast->TempConnection->Destroy();
							WallRefCast->TempConnection = nullptr;
						}
					}
					TempGateWalls[Index]->Destroy();
					TempGateWalls[Index] = nullptr;
				}
			}

			if (AWall* WallCast = Cast<AWall>(LocalObjectSpawn))
			{
				ResetWallPreviewLatch = true;
				WallPreviewArray = (WallCast->FindWallTowersInRange());
				FHitResult Hit;
				TArray<AActor*> ActorsToIgnore;
				for (AWall* TargetWall : WallPreviewArray)
				{
					//if not yet previewed, create a preview and add to existing preview array, need to keep track of connected walls and their order
					FVector Displacement = TargetWall->GetActorLocation() - WallCast->GetActorLocation();
					FVector UnitDirection = Displacement / Displacement.Size();
					if (TargetWall->OwningPlayerID == GetPlayerState()->GetPlayerId() && !
						UKismetSystemLibrary::BoxTraceSingle(
							GetWorld(),
							WallCast->GetActorLocation() + FVector(UnitDirection.X * 24, UnitDirection.Y * 24, 21),
							TargetWall->GetActorLocation() + FVector(UnitDirection.X * -24, UnitDirection.Y * -24, 21),
							FVector(9.5f, 4.0f, 15.0f), Displacement.Rotation(),
							UEngineTypes::ConvertToTraceType(ECC_Visibility), true, ActorsToIgnore,
							EDrawDebugTrace::ForOneFrame, Hit, true))
					{
						if (!TargetWall->TempConnection)
						{
							//create new
							FTransform Transform = FTransform(Displacement.Rotation(),
							                                  WallCast->GetActorLocation() + (Displacement / 2),
							                                  FVector(Displacement.Size() / 19, 1, 1));
							TargetWall->TempConnection = GetWorld()->SpawnActor(
								WallSegmentGhostClass, &Transform, SpawnParams);
							WallsBeingPreviewed.Add(TargetWall);
						}
						else if (TargetWall->TempConnection)
						{
							//update existing
							FVector Displacement = TargetWall->GetActorLocation() - WallCast->GetActorLocation();
							FTransform Transform = FTransform(Displacement.Rotation(),
							                                  WallCast->GetActorLocation() + (Displacement / 2),
							                                  FVector(Displacement.Size() / 19, 1, 1));
							TargetWall->TempConnection->SetActorTransform(Transform);
						}
					}
					else
					{
						if (TargetWall->TempConnection)
						{
							TargetWall->TempConnection->Destroy();
							TargetWall->TempConnection = nullptr;
						}
					}
				}

				for (AWall* WallToCheck : WallsBeingPreviewed)
				{
					if (!WallPreviewArray.Contains(WallToCheck) && WallToCheck->TempConnection)
					{
						WallToCheck->TempConnection->Destroy();
						WallToCheck->TempConnection = nullptr;
						//WallsBeingPreviewed.Remove(WallToCheck);
					}
				}
			}
			else if (ResetWallPreviewLatch)
			{
				for (AWall* Wall : WallsBeingPreviewed)
				{
					if (Wall->TempConnection)
					{
						Wall->TempConnection->Destroy();
						Wall->TempConnection = nullptr;
					}
				}
				WallsBeingPreviewed.Empty();
				ResetWallPreviewLatch = false;
			}
		}
		else if (CursorHit.GetActor())
		{
			//Did hit something and it was a building
			if (ResetGatePreviewLatch)
			{
				ResetGatePreviewLatch = false;
				for (int Index = 0; Index < TempGateWalls.Num(); Index++)
				{
					if (!TempGateWalls[Index]) continue;
					if (AWall* WallRefCast = Cast<AWall>(TempGateWalls[Index]))
					{
						if (WallRefCast->TempConnection)
						{
							WallRefCast->TempConnection->Destroy();
							WallRefCast->TempConnection = nullptr;
						}
					}
					TempGateWalls[Index]->Destroy();
					TempGateWalls[Index] = nullptr;
				}
			}

			for (AWall* Wall : WallsBeingPreviewed)
			{
				if (Wall->TempConnection)
				{
					Wall->TempConnection->Destroy();
					Wall->TempConnection = nullptr;
				}
			}
			WallsBeingPreviewed.Empty();

			if (LocalObjectSpawn->IsA(ATower::StaticClass()) && CursorHit.GetActor()->IsA(ATower::StaticClass()))
			{
				LocalObjectSpawn->SetActorHiddenInGame(true);
				ATower* TempTower = Cast<ATower>(CursorHit.GetActor());
				if (TempTower->OwningPlayerID == GetPlayerState()->GetPlayerId()) TempTower->PreviewActive = true;
				ResetLocalView = true;
			}
			else if (LocalObjectSpawn->IsA(AWorkshop::StaticClass()) && CursorHit.GetActor()->IsA(
				AWorkshop::StaticClass()))
			{
				LocalObjectSpawn->SetActorHiddenInGame(true);
				AWorkshop* TempWorkshop = Cast<AWorkshop>(CursorHit.GetActor());
				if (TempWorkshop->OwningPlayerID == GetPlayerState()->GetPlayerId()) TempWorkshop->PreviewActive = true;
				ResetLocalView = true;
			}
		}
	}
		//Unit Menu not open
	else
	{
		if (ActorDestroyLatch)
		{
			//Do not combine into one if statement, still want to reset latch if object is nullptr
			if (LocalObjectSpawn) LocalObjectSpawn->Destroy();
			for (int Index = 0; Index < TempGateWalls.Num(); Index++)
			{
				if (!TempGateWalls[Index]) continue;
				if (AWall* WallRefCast = Cast<AWall>(TempGateWalls[Index]))
				{
					if (WallRefCast->TempConnection)
					{
						WallRefCast->TempConnection->Destroy();
						WallRefCast->TempConnection = nullptr;
					}
				}
				TempGateWalls[Index]->Destroy();
				TempGateWalls[Index] = nullptr;
			}

			if (WallsBeingPreviewed.Num() != 0)
			{
				for (AWall* Wall : WallsBeingPreviewed)
				{
					if (Wall->TempConnection)
					{
						Wall->TempConnection->Destroy();
						Wall->TempConnection = nullptr;
					}
				}
				WallsBeingPreviewed.Empty();
			}
			ActorSpawnLatch = true;
			ActorDestroyLatch = false;
			PreviewRotation = FRotator::ZeroRotator;
		}
		if (IsLeftMouseDown)
		{
			const TArray<AActor*> RaycastIgnore;
			if (SphereSpawnLatch)
			{
				FHitResult InitRaycastHit;
				const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
				UKismetSystemLibrary::LineTraceSingleForObjects(
					GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
					GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
					UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation()) * 10000.0f,
					StaticObjectTypes, true, RaycastIgnore, EDrawDebugTrace::ForOneFrame, InitRaycastHit, true);
				InitialHit = InitRaycastHit.Location;
				FTransform Transform = FTransform(FRotator::ZeroRotator, InitialHit, FVector(1));
				SelectionSphereHolder = (ASelectionSphere*)(GetWorld()->SpawnActor(
					SelectionSphereClass, &Transform, SpawnParams));
				if (!ShouldAddToSelection) TurnOffAllSelectionCircles();
				SphereDestroyLatch = true;
				ActorSpawnLatch = true;
				SphereSpawnLatch = false;
			}
			FHitResult RaycastHit;
			UKismetSystemLibrary::LineTraceSingleForObjects(
				GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
				GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
				UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation()) * 10000.0f,
				StaticObjectTypes, true, RaycastIgnore, EDrawDebugTrace::ForOneFrame, RaycastHit, true);
			CurrentHit = RaycastHit.Location;
			SphereTraceRadius = (CurrentHit - InitialHit).Size() / 2;
			//i.e. get half length of spanning vector as the radius
			FVector SphereCenter = FVector((CurrentHit.X + InitialHit.X) / 2, (CurrentHit.Y + InitialHit.Y) / 2,
			                               (CurrentHit.Z + InitialHit.Z) / 2);
			UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), SphereCenter, SphereCenter + FVector(0, 0, 10),
			                                                 SphereTraceRadius, DynamicObjectTypes, false,
			                                                 RaycastIgnore, EDrawDebugTrace::ForOneFrame,
			                                                 SphereTraceHits, true);
			if (SelectionSphereHolder)SelectionSphereHolder->SetActorTransform(
				FTransform(FRotator::ZeroRotator, SphereCenter, FVector(SphereTraceRadius / SelectionSphereScaleMod)));
		}
			//Left Mouse button not pressed
		else if (SphereDestroyLatch)
		{
			if (SelectionSphereHolder) SelectionSphereHolder->Destroy();
			CreateSelectedUnitsArray(SphereTraceHits, GetPlayerState()->GetPlayerId(), ShouldAddToSelection);
			ActorSpawnLatch = true;
			SphereSpawnLatch = true;
			SphereDestroyLatch = false;
		}
	}
}

void ASassPlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	//WASD Movement
	InputComponent->BindAxis("MoveForward", this, &ASassPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASassPlayer::MoveRight);
	//Camera Movement
	InputComponent->BindAxis("PitchCamera", this, &ASassPlayer::PitchCamera);
	InputComponent->BindAxis("YawCamera", this, &ASassPlayer::YawCamera);
	//Mouse functions
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &ASassPlayer::LeftClickPressed);
	InputComponent->BindAction("LeftClick", IE_Released, this, &ASassPlayer::LeftClickReleased);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &ASassPlayer::RightClickPressed);
	InputComponent->BindAction("RightClick", IE_Released, this, &ASassPlayer::RightClickReleased);
	//Sprint functions
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ASassPlayer::SprintPressed);
	InputComponent->BindAction("Sprint", IE_Released, this, &ASassPlayer::SprintReleased);
	//Crouch functions
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ASassPlayer::CrouchPressed);
	InputComponent->BindAction("Crouch", IE_Released, this, &ASassPlayer::CrouchReleased);
	//Jump function
	InputComponent->BindAction("Jump", IE_Pressed, this, &ASassPlayer::JumpPressed);
	//Pause function
	InputComponent->BindAction("Pause", IE_Pressed, this, &ASassPlayer::PausePressed);
	//Quit function
	InputComponent->BindAction("ForceQuit", IE_Pressed, this, &ASassPlayer::QuitGame);
	//UnitMenu function
	InputComponent->BindAction("UnitMenu", IE_Pressed, this, &ASassPlayer::UnitMenuPressed);
	//test functions, default L
	InputComponent->BindAction("Test", IE_Pressed, this, &ASassPlayer::testFunction);
}

void ASassPlayer::testFunction()
{
	FHitResult Hit;
	if (!PlayerControllerPtr) return;
	PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit);
	// Assign Hit
	if (!Hit.GetActor()) return;
	FVector Location = Hit.GetActor()->GetActorLocation();
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, UKismetStringLibrary::Conv_VectorToString(Location));
}


void ASassPlayer::LeftClickPressed()
{
	IsLeftMouseDown = true;
	if (!IsRightMouseDown)
	{
		if (IsUnitMenuOpen)
		{
			if (PlayerControllerPtr == nullptr) return;
			FHitResult Hit;
			TArray<FVector> LocationsToCheck;
			TArray<AActor*> ActorsToIgnore = TArray<AActor*>();

			PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),
			                                                      true, Hit); // Assign Hit

			if (ABuildingBase* LocalBuildingRef = Cast<ABuildingBase>(LocalObjectSpawn))
			{
				LocationsToCheck = LocalBuildingRef->CornerLocations;
				if (SelectedSpawnableType == ETypeOfSpawnable::BUILDING_GATE) { ActorsToIgnore = TempGateWalls; }
			}

			const TArray<AActor*> ConfirmedToIgnore = ActorsToIgnore;
			ServerSpawnBuilding(Cast<ASassPlayerController>(PlayerControllerPtr), SelectedSpawnableClass, Hit,
			                    PreviewRotation, CurrentHalfHeight, LocationsToCheck, CurrentTraceSize,
			                    GetPlayerState()->GetPlayerId(), ConfirmedToIgnore);
		}
	}
}

void ASassPlayer::LeftClickReleased()
{
	IsLeftMouseDown = false;
}

void ASassPlayer::RightClickPressed()
{
	IsRightMouseDown = true;

	FHitResult RaycastHit;
	const TArray<AActor*> RaycastIgnores;

	if (!IsUnitMenuOpen)
	{
		UKismetSystemLibrary::LineTraceSingle(
			GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
			GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
			UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation()) * 10000.0f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::ForDuration,
			RaycastHit, true);
		if (RaycastHit.GetComponent()->ComponentHasTag(USassCStaticLibrary::NoAggroTag()))
		{
			UKismetSystemLibrary::LineTraceSingle(
				GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
				GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
				UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation()) * 10000.0f,
				UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::ForDuration,
				RaycastHit, true);
		}
		AActor* HitActor = RaycastHit.GetActor();
		ETypeOfOrder OrderType = ETypeOfOrder::ORDER_WORLD;
		if ((HitActor->IsA(AUnitBase::StaticClass()) || HitActor->IsA(ASelectionSphere::StaticClass())) && Cast<
			AUnitBase>(HitActor)->OwningPlayerID != GetPlayerState()->GetPlayerId())
		{
			OrderType = ETypeOfOrder::ORDER_UNIT;
		}
		if (HitActor->IsA(ABuildingBase::StaticClass()) && Cast<ABuildingBase>(HitActor)->OwningPlayerID !=
			GetPlayerState()->GetPlayerId()) { OrderType = ETypeOfOrder::ORDER_BUILDING; }
		CommandUnits(SelectedUnits, RaycastHit, OrderType);
	}
}

void ASassPlayer::RightClickReleased()
{
	IsRightMouseDown = false;
}

void ASassPlayer::CommandUnits_Implementation(const TArray<AUnitBase*>& SelectedUnits, FHitResult RaycastHit,
                                              ETypeOfOrder OrderType)
{
	switch (OrderType)
	{
	case ETypeOfOrder::ORDER_UNIT:
		for (AUnitBase* Unit : SelectedUnits) { if (Unit) Unit->MoveToUnit(RaycastHit.GetActor()); }
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, "SassPlayer CommandUnits: Attack a unit");
		break;
	case ETypeOfOrder::ORDER_BUILDING:
		for (AUnitBase* Unit : SelectedUnits) { if (Unit) Unit->MoveToBuilding(RaycastHit.GetActor()); }
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, "SassPlayer CommandUnits: Attack a building");
		break;
	case ETypeOfOrder::ORDER_WORLD:
	default:
		for (AUnitBase* Unit : SelectedUnits) { if (Unit) Unit->MoveToDest(RaycastHit.Location); }
		break;
	}
}

bool ASassPlayer::CommandUnits_Validate(const TArray<AUnitBase*>& SelectedUnits, FHitResult RaycastHit,
                                        ETypeOfOrder OrderType)
{
	return true;
}

#pragma region HUD functions
void ASassPlayer::UnitMenuPressed()
{
	if (!IsUnitMenuOpen)
	{
		if (PlayerControllerPtr != nullptr)
		{
			PlayerControllerPtr->bShowMouseCursor = true;
			PlayerControllerPtr->SetInputMode(FInputModeGameAndUI());
			OpenUnitMenu();
			UpdateSelectedSpawnableClass(SelectedSpawnableClass, SelectedSpawnableType);
		}
		IsUnitMenuOpen = true;
	}
	else
	{
		if (PlayerControllerPtr != nullptr)
		{
			PlayerControllerPtr->bShowMouseCursor = false;
			PlayerControllerPtr->SetInputMode(FInputModeGameOnly());
			CloseUnitMenu();
		}
		IsUnitMenuOpen = false;
	}
}

void ASassPlayer::CreatePregameHUD()
{
	if (PlayerControllerPtr->IsLocalController() && PregameWidget == nullptr)
	{
		PregameWidget = CreateWidget<UUserWidget>(PlayerControllerPtr, PregameWidgetClass);
		if (PregameWidget != nullptr) PregameWidget->AddToViewport();
	}
}

void ASassPlayer::RemoveAllWidgets()
{
	for (TObjectIterator<UUserWidget> WidgetIter; WidgetIter; ++WidgetIter)
	{
		UUserWidget* CurrentWidget = *WidgetIter;
		if (!CurrentWidget->GetWorld()) continue;
		else CurrentWidget->RemoveFromParent();
	}
}

void ASassPlayer::CreateGameHUD()
{
	if (PlayerControllerPtr->IsLocalController())
	{
		RemoveAllWidgets();
		GameWidget = CreateWidget<UUserWidget>(PlayerControllerPtr, GameWidgetClass);
		if (GameWidget != nullptr) GameWidget->AddToViewport();
		PlayerControllerPtr->SetInputMode(FInputModeGameOnly());
		PlayerControllerPtr->bShowMouseCursor = false;
		SetSassHUDRef();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
		                                 "SassPlayer Creategamehud - local controller not found");
	}
}

UUserWidget* ASassPlayer::GetGameWidget()
{
	return GameWidget;
}
#pragma endregion

void ASassPlayer::QuitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ASassPlayer::UpdateSelectedSpawnableClass(UClass* NewClass, ETypeOfSpawnable NewTypeOfSpawnable)
{
	if (!NewClass) return;
	SelectedSpawnableClass = NewClass;
	SelectedSpawnableType = NewTypeOfSpawnable;
	UObject* NewActor = SelectedSpawnableClass->GetDefaultObject();

	if (NewActor->IsA(ABuildingBase::StaticClass()))
	{
		ABuildingBase* SelectedBuilding = Cast<ABuildingBase>(NewActor);
		CurrentHalfHeight = SelectedBuilding->HalfHeight;
		CurrentTraceSize = SelectedBuilding->TraceSize;
	}
	else if (NewActor->IsA(AUnitBase::StaticClass()))
	{
		AUnitBase* SelectedUnit = Cast<AUnitBase>(NewActor);
		CurrentHalfHeight = FVector(0, 0, SelectedUnit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		CurrentTraceSize = SelectedUnit->TraceSize;
	}
}

#pragma region Pause functions
void ASassPlayer::PausePressed()
{
	if (!IsPaused)
	{
		if (PauseWidgetClass != nullptr) PauseWidget = CreateWidget<USassPauseMenu>(
			PlayerControllerPtr, PauseWidgetClass);
		if (PauseWidget != nullptr) PauseWidget->AddToViewport();
		if (PlayerControllerPtr != nullptr)
		{
			PlayerControllerPtr->SetInputMode(FInputModeGameAndUI());
			PlayerControllerPtr->SetIgnoreMoveInput(true);
			PlayerControllerPtr->bShowMouseCursor = true;
		}
		IsPaused = true;
	}
	else
	{
		if (PauseWidget != nullptr)
		{
			PauseWidget->RemoveFromViewport();
			PauseWidget = nullptr;
		}
		if (PlayerControllerPtr != nullptr)
		{
			PlayerControllerPtr->SetInputMode(FInputModeGameOnly());
			PlayerControllerPtr->SetIgnoreMoveInput(false);
			PlayerControllerPtr->bShowMouseCursor = false;
		}
		IsPaused = false;
	}
}

void ASassPlayer::ChangePauseWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (PauseWidget != nullptr)
	{
		PauseWidget->RemoveFromViewport();
		PauseWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		PauseWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (PauseWidget != nullptr) PauseWidget->AddToViewport();
	}
}
#pragma endregion

#pragma region Jump function
void ASassPlayer::JumpPressed()
{
	Jump();
}
#pragma endregion

#pragma region Crouch functions
void ASassPlayer::CrouchPressed()
{
	BaseEyeHeight = CrouchingEyeHeight;
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	ServerCrouch(true, GetCharacterMovement());
}

void ASassPlayer::CrouchReleased()
{
	BaseEyeHeight = StandingEyeHeight;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ServerCrouch(false, GetCharacterMovement());
}

void ASassPlayer::ServerCrouch_Implementation(bool isCrouching, UCharacterMovementComponent* movementComponent)
{
	if (isCrouching) { movementComponent->MaxWalkSpeed = CrouchSpeed; }
	else { movementComponent->MaxWalkSpeed = WalkSpeed; }
}

bool ASassPlayer::ServerCrouch_Validate(bool isCrouching, UCharacterMovementComponent* movementComponent)
{
	return true;
}
#pragma endregion

#pragma region Sprint functions
void ASassPlayer::SprintPressed()
{
	ShouldAddToSelection = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	ServerSprint(true, GetCharacterMovement());
}

void ASassPlayer::SprintReleased()
{
	ShouldAddToSelection = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ServerSprint(false, GetCharacterMovement());
}

void ASassPlayer::ServerSprint_Implementation(bool isRunning, UCharacterMovementComponent* movementComponent)
{
	if (isRunning) movementComponent->MaxWalkSpeed = SprintSpeed;
	else movementComponent->MaxWalkSpeed = WalkSpeed;
}

bool ASassPlayer::ServerSprint_Validate(bool isRunning, UCharacterMovementComponent* movementComponent)
{
	return true;
}
#pragma endregion

#pragma region WASD Movement
void ASassPlayer::MoveForward(float AxisValue)
{
	if (Controller != NULL && AxisValue != 0.0f)
	{
		FRotator Rotation = GetMesh()->GetComponentRotation() - GetMesh()->GetRelativeRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void ASassPlayer::MoveRight(float AxisValue)
{
	if (Controller != NULL && AxisValue != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}
#pragma endregion

#pragma region Camera Movement
void ASassPlayer::PitchCamera(float AxisValue)
{
	if (Controller != NULL && AxisValue != 0.0f)
	{
		if (InvertPitch)
		{
			AddControllerPitchInput(-AxisValue);
		}
		else
		{
			AddControllerPitchInput(AxisValue);
		}
	}
}

void ASassPlayer::YawCamera(float AxisValue)
{
	if (Controller != NULL && AxisValue != 0.0f)
	{
		if (InvertYaw)
		{
			AddControllerYawInput(-AxisValue);
		}
		else
		{
			AddControllerYawInput(AxisValue);
		}
	}
}

bool ASassPlayer::ShouldIgnoreLookInput()
{
	return (IsUnitMenuOpen && IsRightMouseDown);
}
#pragma endregion

#pragma region Unit Interaction
void ASassPlayer::TurnOffAllSelectionCircles()
{
	if (GetPlayerState() == nullptr) { return; }
	for (AActor* UnitIt : ((GetPlayerState<ASassPlayerState>())->ControlledBuildings))
	{
		AUnitBase* Unit = Cast<AUnitBase>(UnitIt);
		if (Unit != nullptr) { Unit->SetDecalVisibility(false); }
	}
}

void ASassPlayer::CreateSelectedUnitsArray(TArray<FHitResult> Hits, int32 PlayerID, bool AddToSelection)
{
	if (!AddToSelection)
	{
		TurnOffAllSelectionCircles();
		SelectedUnits.Empty();
	}
	ASassPlayerState* TempPlayerState = GetPlayerState<ASassPlayerState>();

	if (TempPlayerState == nullptr) { return; } //Shouldn't happen

	for (FHitResult Hit : Hits)
	{
		AUnitBase* Unit = Cast<AUnitBase>(Hit.GetActor());


		if (Unit && !Hit.GetComponent()->ComponentHasTag(USassCStaticLibrary::NoAggroTag()) && (Unit->OwningPlayerID ==
			PlayerID))
		{
			SelectedUnits.Add(Unit);
			Unit->SetDecalVisibility(true);
		}
	}

	TempPlayerState->SelectedUnits = SelectedUnits;
}

AActor* ASassPlayer::GetSelectionSphereHolder()
{
	return SelectionSphereHolder;
}

#pragma endregion

#pragma region Server-side Spawning
void ASassPlayer::ServerSpawnBuilding_Implementation(ASassPlayerController* PlayerController,
                                                     TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, FRotator Rotator,
                                                     const FVector& HalfHeight, const TArray<FVector>& Midpoints,
                                                     const FVector& TraceSize, int32 PlayerID,
                                                     const TArray<AActor*>& ActorsToIgnore)
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters WallParams = FActorSpawnParameters(TempParams);

	const FVector Location = Hit.Location + HalfHeight;
	const FRotator Rotation = Rotator;

	bool SpawningBuilding = ActorToSpawn.GetDefaultObject()->IsA(ABuildingBase::StaticClass());

	if (Hit.GetActor() && !Hit.GetActor()->IsA(ABuildingBase::StaticClass()))
	{
		if (Hit.Normal.Z >= .990)
		{
			const TArray<AActor*> BoxIgnore;
			FHitResult BoxHit;

			if (!UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Hit.Location + FVector(0, 0, 2),
			                                          Hit.Location + 2 * HalfHeight,
			                                          FVector(TraceSize.X, TraceSize.Y, 0), Rotation,
			                                          UEngineTypes::ConvertToTraceType(
				                                          ECollisionChannel::ECC_GameTraceChannel2), true,
			                                          ActorsToIgnore, EDrawDebugTrace::ForDuration, BoxHit, true))
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red,
				                                 UKismetStringLibrary::Conv_VectorToString(Location));
				//if there is no hit (good)
				if ((ActorToSpawn.GetDefaultObject()->IsA(ABuildingBase::StaticClass()) && !CheckBldgCorners(
						Midpoints, Hit.Location, PreviewRotation, PlayerID,
						ActorToSpawn.GetDefaultObject()->IsA(ACity::StaticClass())))
					|| ((ActorToSpawn.GetDefaultObject()->IsA(AUnitBase::StaticClass())) && !CheckUnitLocation(
						Hit.Location, PreviewRotation, PlayerID)))
				{
					//if there is no obstruction (good)
					AActor* NewSpawn = GetWorld()->SpawnActor(ActorToSpawn, &Location, &Rotation, SpawnParams);

					if (NewSpawn == nullptr)
					{
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
						                                 "SassPlayer ServerSpawnBuilding: Could not spawn, unknown reason");
						return;
					}

					if (SpawningBuilding) { NewSpawn->SetActorLocation(Hit.Location); }
					ASassPlayerState* SassPlayerState = GetPlayerState<ASassPlayerState>();
					if (SassPlayerState != nullptr)
					{
						SassPlayerState->ControlledBuildings.Add(NewSpawn);
						NewSpawn->SetOwner(PlayerController);
					}

					//@TODO: Should find a way to consolidate these... hard with units being ACharacter, buildings being AActor to give them a common unit
					//try unit (more likely)
					if (AUnitBase* NewUnit = Cast<AUnitBase>(NewSpawn))
					{
						NewUnit->UpdateMaterial(SassPlayerState->PlayerColor);
						NewUnit->SpawnDefaultController();
						NewUnit->OwningPlayerID = PlayerID;

						//Don't think units need to have location fixed. Small change shouldn't matter. If it does an alternative solution will need to be found, as this causes issues.
						//NewUnit->FixSpawnLocation(Location);
					}
						//try building
					else
					{
						if (ABuildingBase* NewBuilding = Cast<ABuildingBase>(NewSpawn))
						{
							NewBuilding->UpdateMaterial(SassPlayerState->PlayerColor, true);
							NewBuilding->OwningPlayerID = PlayerID;
							NewBuilding->PostCreation(SassPlayerState->PlayerColor);
							NewBuilding->FixSpawnLocation(Hit.Location);
							if (AWall* WallCast = Cast<AWall>(NewBuilding))
							{
								TArray<AWall*> WallsInRange = WallCast->FindWallTowersInRange();
								for (AWall* TargetWall : WallsInRange)
								{
									if (TargetWall->OwningPlayerID == PlayerID)
									{
										ServerSpawnWall(WallCast, TargetWall, PlayerID, SassPlayerState->PlayerColor);
									}
								}
							}
							else if (AGate* GateCast = Cast<AGate>(NewBuilding))
							{
								//@TODO Check if I can skip AActor* and just spawn in with Ignore.Add(Getworld. . .  .
								const FVector LeftLoc = Hit.Location + HalfHeight + (Rotator + FRotator(0, 90, 0)).
									Vector() * 50 + FVector(0, 0, -20);
								const FVector RightLoc = Hit.Location + HalfHeight + (Rotator + FRotator(0, 90, 0)).
									Vector() * -50 + FVector(0, 0, -20);
								AActor* x = GetWorld()->SpawnActor(WallClass, &LeftLoc, &Rotation, WallParams);
								AActor* y = GetWorld()->SpawnActor(WallClass, &RightLoc, &Rotation, WallParams);
								TArray<AActor*> Ignore = TArray<AActor*>(ActorsToIgnore);
								Ignore.Add(x);
								Ignore.Add(y);

								for (int SideNum = 0; SideNum < Ignore.Num(); SideNum++)
								{
									if (!Ignore[SideNum]) continue;
									AWall* Wall = Cast<AWall>(Ignore[SideNum]);
									Wall->OwningPlayerID = PlayerID;
									Wall->UpdateMaterial(SassPlayerState->PlayerColor, true);
									Wall->PostCreation(SassPlayerState->PlayerColor);
									ServerSpawnWall(Wall, Wall->GetClosestWallTowerInRange(100.0f, Ignore), PlayerID,
									                SassPlayerState->PlayerColor);
								}
							}
							else if (AShieldMonolith* MonoCast = Cast<AShieldMonolith>(NewBuilding))
							{
								TArray<AShieldMonolith*> MonolithsInRange = MonoCast->FindMonolithsInRange();
								if (MonolithsInRange.Num() > 0)
								{
									UParticleSystem* PSysToSpawn = MonoCast->BeamPSys;
									for (AShieldMonolith* TargetMonolith : MonolithsInRange)
									{
										TargetMonolith->SpawnBeamEmitter(PSysToSpawn, MonoCast, TargetMonolith);
									}
								}
							}
						}
						else
						{
							GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
							                                 "SassPlayer ServerSpawnBuilding: Could not spawn, server could not determine what spawn was being asked for");
						}
					}
				}
				else
				{
					//Bad Spawn
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
					                                 "SassPlayer ServerSpawnBuilding: Could not spawn because corners were obstructed");
				}
			}
			else
			{
				//Bad Spawn
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
				                                 "SassPlayer ServerSpawnBuilding: Could not spawn because trace hit something");
			}
		}
		else
		{
			//Bad Spawn
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
			                                 "SassPlayer ServerSpawnBuilding: Could not spawn because location uneven");
		}
	}
	else if (Hit.GetActor())
	{
		if (ActorToSpawn.GetDefaultObject()->IsA(ATower::StaticClass()) && Hit.GetActor()->IsA(ATower::StaticClass()))
		{
			ATower* TempTower = Cast<ATower>(Hit.GetActor());
			if (TempTower->OwningPlayerID == PlayerID) TempTower->UpgradeBuilding();
		}
		else if (ActorToSpawn.GetDefaultObject()->IsA(AWorkshop::StaticClass()) && Hit.GetActor()->IsA(
			AWorkshop::StaticClass()))
		{
			AWorkshop* TempWorkshop = Cast<AWorkshop>(Hit.GetActor());
			if (TempWorkshop->OwningPlayerID == PlayerID) TempWorkshop->UpgradeBuilding();
		}
	}
}

bool ASassPlayer::ServerSpawnBuilding_Validate(ASassPlayerController* PlayerController,
                                               TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, FRotator Rotator,
                                               const FVector& HalfHeight, const TArray<FVector>& Midpoints,
                                               const FVector& TraceSize, int32 PlayerID,
                                               const TArray<AActor*>& ActorsToIgnore)
{
	return true;
}
#pragma endregion

void ASassPlayer::GetAllPlayerColors()
{
	TArray<AActor*> FoundControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerControllerClass, FoundControllers);
	for (AActor* Controller : FoundControllers)
	{
	}
}

void ASassPlayer::ColorPlayer_Implementation(FLinearColor PlayerColor)
{
	if (DynamicPlayerMaterial == nullptr || GetMesh() == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "PlayerMaterial or Mesh bad");
		return;
	}
	DynamicPlayerMaterial->SetVectorParameterValue(ColorParameterName, PlayerColor);
	GetMesh()->SetMaterial(0, DynamicPlayerMaterial);
}

bool ASassPlayer::ColorPlayer_Validate(FLinearColor PlayerColor)
{
	return true;
}

bool ASassPlayer::CheckUnitLocation(FVector Center, FRotator Rotator, int32 PlayerID)
{
	TArray<AActor*> nullArray;
	FHitResult Hit;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Center + FVector(0, 0, 55.0f), Center - FVector(0, 0, 15.0f),
	                                          UEngineTypes::ConvertToTraceType(
		                                          ECollisionChannel::ECC_GameTraceChannel1), false, nullArray,
	                                          EDrawDebugTrace::ForOneFrame, Hit, true))
	{
		if (ABuildingBase* Bldg = Cast<ABuildingBase>(Hit.GetActor()))
		{
			if (Bldg->OwningPlayerID == PlayerID)
			{
				GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald,
				                                 "SassPlayer CheckUnitLocation: good spawn inside TERRITORY");
				return false;
			}
		}
	}
	return true;
}

bool ASassPlayer::CheckBldgCorners(TArray<FVector> ExtraLocs, FVector Center, FRotator Rotator, int32 PlayerID,
                                   bool isCity)
{
	if (ExtraLocs.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No extre locs");
		return true;
	}

	FHitResult Hit;
	FVector DirectionUnitVector = (Rotator).Vector();
	TArray<float> TraceHeights;
	TArray<AActor*> Ignore;

	for (FVector Loc : ExtraLocs)
	{
		float Magnitude = Loc.Size2D();
		FVector Displacement = (Loc.Rotation() + Rotator).Vector() * Magnitude;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Center + Displacement + FVector(0, 0, 15.0f),
		                                                    Center + Displacement - FVector(0, 0, 15.0f),
		                                                    StaticObjectTypes, true, Ignore,
		                                                    EDrawDebugTrace::ForOneFrame, Hit, true))
		{
			TraceHeights.Add(Hit.Location.Z);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald,
			                                 "SassPlayer CheckBldgCorners: TRACE MADE NO CONTACT");
			return true;
		}
	}
	float first = TraceHeights[0];
	for (float Height : TraceHeights)
	{
		if (FMath::Abs(Height - first) > MAX_BLDG_CORNER_DIFFERENCE)
		{
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald,
			                                 "SassPlayer CheckBldgCorners: HEIGHTS NOT EQUAL");
			return true;
		}
	}


	TArray<AActor*> nullArray;
	for (FVector Loc : ExtraLocs)
	{
		float Magnitude = Loc.Size2D();
		FVector Displacement = (Loc.Rotation() + Rotator).Vector() * Magnitude;
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Center + Displacement + FVector(0, 0, 65.0f),
		                                          Center + Displacement - FVector(0, 0, 15.0f),
		                                          UEngineTypes::ConvertToTraceType(
			                                          ECollisionChannel::ECC_GameTraceChannel1), false, nullArray,
		                                          EDrawDebugTrace::ForOneFrame, Hit, true))
		{
			if (ABuildingBase* Bldg = Cast<ABuildingBase>(Hit.GetActor()))
			{
				if (Bldg->OwningPlayerID != PlayerID)
				{
					GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald,
					                                 "SassPlayer CheckBldgCorners: OVERLAPS ENEMY TERRITORY");
					return true;
				}
			}
		}
		else if (!isCity)
		{
			//Trace did not hit
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Green,
			                                 "SassPlayer CheckBldgCorners: TERRITORY TRACE DID NOT MAKE CONTACT");
			return true;
		}
	}

	if (isCity)
	{
		TArray<FHitResult> SphereHits;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Center, Center + FVector(0, 0, 1),
		                                       USassCStaticLibrary::CityDefaultInfluenceRange(),
		                                       UEngineTypes::ConvertToTraceType(
			                                       ECollisionChannel::ECC_GameTraceChannel1), false, nullArray,
		                                       EDrawDebugTrace::ForOneFrame, SphereHits, true);
		for (FHitResult AHit : SphereHits)
		{
			if (ABuildingBase* Bldg = Cast<ABuildingBase>(AHit.GetActor()))
			{
				if (Bldg->OwningPlayerID != PlayerID)
				{
					GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Orange,
					                                 "SassPlayer CheckBldgCorners: CITY TERRITORY OVERLAPS ENEMY TERRITORY");
					return true;
				}
			}
		}
	}

	return false;
}

void ASassPlayer::LateStart_Implementation(APlayerController* NewPlayer)
{
}

bool ASassPlayer::LateStart_Validate(APlayerController* NewPlayer)
{
	return true;
}


void ASassPlayer::SpawnWallPreview_Implementation(FVector Location, FRotator Rotation)
{
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	GetWorld()->SpawnActor(WallSegmentClass, &Location, &Rotation, SpawnParams);
}

void ASassPlayer::ServerSpawnWall_Implementation(AWall* NewWall, AWall* TargetWall, int32 PlayerID,
                                                 FLinearColor PlayerColor)
{
	if (!TargetWall)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "ServerSpawnWall: TargetWall does not exist");
		return;
	}
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;

	FVector Direction = NewWall->GetActorLocation() - TargetWall->GetActorLocation();
	FVector UnitDirection = Direction / Direction.Size();

	if (!UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(), NewWall->GetActorLocation() + FVector(UnitDirection.X * -24, UnitDirection.Y * -24, 21),
		TargetWall->GetActorLocation() + FVector(UnitDirection.X * 24, UnitDirection.Y * 24, 21),
		FVector(9.5f, 4.0f, 15.0f), Direction.Rotation(), UEngineTypes::ConvertToTraceType(ECC_Visibility), true,
		ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true))
	{
		float SpaceBetween = ((TargetWall->GetActorLocation() - FVector(Direction.X / Direction.Size2D() * -24,
		                                                                Direction.Y / Direction.Size2D() * -24, 0)) - (
			NewWall->GetActorLocation() + FVector(Direction.X / Direction.Size2D() * 24,
			                                      Direction.Y / Direction.Size2D() * 24, 0))).Size();
		FVector Start = NewWall->GetActorLocation() + FVector(UnitDirection.X * -18, UnitDirection.Y * -18, 0);
		bool FirstLoop = true, LastLoop = false;
		AWallSegment* PreviousSegment = nullptr;
		for (int NumToSpawn = (SpaceBetween / 19); NumToSpawn > 0; NumToSpawn--)
		{
			const FVector Loc = Start;
			const FRotator Rot = Direction.Rotation();
			AWallSegment* NewSegment = Cast<AWallSegment>(
				GetWorld()->SpawnActor(WallSegmentClass, &Loc, &Rot, SpawnParams));
			NewSegment->OwningPlayerID = PlayerID;
			NewSegment->UpdateMaterial(PlayerColor, true);
			NewSegment->FixSpawnLocation(Loc);
			if (FirstLoop)
			{
				NewWall->AddConnectedWallSegment(NewSegment);
				NewSegment->LeftConnection = NewWall;
				FirstLoop = false;
			}
			else if (LastLoop)
			{
				PreviousSegment->RightConnection = NewSegment;
				NewSegment->LeftConnection = PreviousSegment;
				NewSegment->RightConnection = TargetWall;
				TargetWall->AddConnectedWallSegment(NewSegment);
			}
			else
			{
				NewSegment->LeftConnection = PreviousSegment;
				PreviousSegment->RightConnection = NewSegment;
			}
			if (NumToSpawn == 2) { LastLoop = true; }

			PreviousSegment = NewSegment;
			Start = Start + FVector(UnitDirection.X * -19, UnitDirection.Y * -19, 0);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "ServerSpawnWall: Spawn was blocked");
	}
}

bool ASassPlayer::ServerSpawnWall_Validate(AWall* NewWall, AWall* TargetWall, int32 PlayerID, FLinearColor PlayerColor)
{
	return true;
}

#pragma region Blueprint Implementables
void ASassPlayer::SetSassHUDRef_Implementation()
{
}

void ASassPlayer::OpenUnitMenu_Implementation()
{
}

void ASassPlayer::CloseUnitMenu_Implementation()
{
}
#pragma endregion
