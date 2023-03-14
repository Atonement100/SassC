// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SassPlayer.h"
#include "BlueprintEditor.h"
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
#include "Core/ChannelDefines.h"
#include "Gamemode/Sassilization/SassGameState.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h" //necessary only for debugging
#include "Kismet/KismetSystemLibrary.h"
#include "Gamemode/Sassilization/SassGameManager.h"
#include "Player/SassPlayerController.h"
#include "Player/SelectionSphere.h"
#include "UI/SassPauseMenu.h"
#include "Units/UnitBase.h"
#include "Net/UnrealNetwork.h"

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
	PlayerControllerPtr = Cast<ASassPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void ASassPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASassPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
	{
		return;
	}
	
	ASassPlayerState* SassPlayerState = GetPlayerState<ASassPlayerState>();
	
	if (PlayerControllerPtr->IsUnitMenuOpen())
	{
		FHitResult CursorHit;

		if (LocalObjectSpawn &&
			Cast<IEntityInterface>(LocalObjectSpawn)->GetTypeOfEntity() != SassPlayerState->GetSelectedTypeOfEntity())
		{
			LocalObjectSpawn->Destroy();
			ActorSpawnLatch = true;
			UE_LOG(Sassilization, Display, TEXT("Resetting ActorSpawnLatch"))
		}

		PlayerControllerPtr->GetHitResultUnderCursorByChannel(
			UEngineTypes::ConvertToTraceType(ECC_WorldStatic), true, CursorHit);
		
		if (ActorSpawnLatch)
		{
			// When this moves to PlayerController it will make a little more sense..
			LocalObjectSpawn = PlayerControllerPtr->GetSassGameManager()->RequestGhostSpawn(PlayerControllerPtr,
				SassPlayerState->GetSelectedTypeOfEntity(), CursorHit.Location, PreviewRotation);
			
			if (SelectionSphereHolder) SelectionSphereHolder->Destroy();
			SphereSpawnLatch = true;
			ActorDestroyLatch = true;
			ActorSpawnLatch = false;
		}
		
		if (LocalObjectSpawn)
		{
			if (!IsRightMouseDown)
			{
				LocalObjectSpawn->SetActorLocation(CursorHit.Location + Cast<IEntityInterface>(LocalObjectSpawn)->GetSpawnOffset());
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

			IsBadSpawn = !PlayerControllerPtr->IsSpawnableRequestValid(LocalObjectSpawn, CursorHit.Location, PreviewRotation);

			FLinearColor NewColor = IsBadSpawn ? FLinearColor(.7f, 0.0f, .058f) : FLinearColor(.093f, .59f, .153f);

			if (ABuildingBase* BuildingCast = Cast<ABuildingBase>(LocalObjectSpawn))
			{
				BuildingCast->UpdateMaterial(NewColor, false);
			}
			else if (AUnitBase* UnitCast = Cast<AUnitBase>(LocalObjectSpawn)) { UnitCast->UpdateMaterial(NewColor); }

			FActorSpawnParameters SpawnParams = FActorSpawnParameters();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
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
							FVector LocalDisplacement = TargetWall->GetActorLocation() - WallInstance->GetActorLocation();
							FTransform Transform = FTransform(LocalDisplacement.Rotation(),
							                                  WallInstance->GetActorLocation() + (LocalDisplacement / 2),
							                                  FVector(LocalDisplacement.Size() / 19, 1, 1));
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
							FVector LocalDisplacement = TargetWall->GetActorLocation() - WallCast->GetActorLocation();
							FTransform Transform = FTransform(LocalDisplacement.Rotation(),
							                                  WallCast->GetActorLocation() + (LocalDisplacement / 2),
							                                  FVector(LocalDisplacement.Size() / 19, 1, 1));
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
				SelectionSphereHolder = Cast<ASelectionSphere>((GetWorld()->SpawnActor(
					SelectionSphereClass, &Transform, SpawnParams)));
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

void ASassPlayer::SetupPlayerInputComponent(class UInputComponent* SetupInputComponent)
{
	Super::SetupPlayerInputComponent(SetupInputComponent);
	//WASD Movement
	SetupInputComponent->BindAxis("MoveForward", this, &ASassPlayer::MoveForward);
	SetupInputComponent->BindAxis("MoveRight", this, &ASassPlayer::MoveRight);
	//Camera Movement
	SetupInputComponent->BindAxis("PitchCamera", this, &ASassPlayer::PitchCamera);
	SetupInputComponent->BindAxis("YawCamera", this, &ASassPlayer::YawCamera);
	//Mouse functions
	SetupInputComponent->BindAction("LeftClick", IE_Pressed, this, &ASassPlayer::LeftClickPressed);
	SetupInputComponent->BindAction("LeftClick", IE_Released, this, &ASassPlayer::LeftClickReleased);
	SetupInputComponent->BindAction("RightClick", IE_Pressed, this, &ASassPlayer::RightClickPressed);
	SetupInputComponent->BindAction("RightClick", IE_Released, this, &ASassPlayer::RightClickReleased);
	//Sprint functions
	SetupInputComponent->BindAction("Sprint", IE_Pressed, this, &ASassPlayer::SprintPressed);
	SetupInputComponent->BindAction("Sprint", IE_Released, this, &ASassPlayer::SprintReleased);
	//Crouch functions
	SetupInputComponent->BindAction("Crouch", IE_Pressed, this, &ASassPlayer::CrouchPressed);
	SetupInputComponent->BindAction("Crouch", IE_Released, this, &ASassPlayer::CrouchReleased);
	//Jump function
	SetupInputComponent->BindAction("Jump", IE_Pressed, this, &ASassPlayer::JumpPressed);
	//Pause function
	SetupInputComponent->BindAction("Pause", IE_Pressed, this, &ASassPlayer::PausePressed);
	//Quit function
	SetupInputComponent->BindAction("ForceQuit", IE_Pressed, this, &ASassPlayer::QuitGame);
	//test functions, default L
	SetupInputComponent->BindAction("Test", IE_Pressed, this, &ASassPlayer::testFunction);
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
		if (PlayerControllerPtr->IsUnitMenuOpen())
		{
			FHitResult Hit;
			PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true, Hit);
			PlayerControllerPtr->ServerRequestBuildingSpawn(GetPlayerState<ASassPlayerState>()->GetSelectedTypeOfEntity(), Hit.Location, PreviewRotation);
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

	if (!PlayerControllerPtr->IsUnitMenuOpen())
	{
		UKismetSystemLibrary::LineTraceSingle(
			GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
			GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
			UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation()) * 10000.0f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::ForDuration,
			RaycastHit, true);
		if (RaycastHit.GetComponent() && RaycastHit.GetComponent()->ComponentHasTag(USassCStaticLibrary::NoAggroTag()))
		{
			UKismetSystemLibrary::LineTraceSingle(
				GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f),
				GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) +
				UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation()) * 10000.0f,
				UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::ForDuration,
				RaycastHit, true);
		}
		AActor* HitActor = RaycastHit.GetActor();
		ETypeOfOrder OrderType = ETypeOfOrder::Order_World;
		if (HitActor)
		{
			if ((HitActor->IsA(AUnitBase::StaticClass()) || HitActor->IsA(ASelectionSphere::StaticClass())) &&
				Cast<AUnitBase>(HitActor)->OwningPlayerID != GetPlayerState()->GetPlayerId())
			{
				OrderType = ETypeOfOrder::Order_Unit;
			}
			if (HitActor->IsA(ABuildingBase::StaticClass()) && Cast<ABuildingBase>(HitActor)->OwningPlayerID !=
				GetPlayerState()->GetPlayerId()) { OrderType = ETypeOfOrder::Order_Building; }
			CommandUnits(SelectedUnits, RaycastHit, OrderType);
		}
	}
}

void ASassPlayer::RightClickReleased()
{
	IsRightMouseDown = false;
}

void ASassPlayer::CommandUnits_Implementation(const TArray<AUnitBase*>& UnitsToCommand, FHitResult RaycastHit,
                                              ETypeOfOrder OrderType)
{
	switch (OrderType)
	{
	case ETypeOfOrder::Order_Unit:
		for (AUnitBase* Unit : UnitsToCommand) { if (Unit) Unit->MoveToUnit(RaycastHit.GetActor()); }
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, "SassPlayer CommandUnits: Attack a unit");
		break;
	case ETypeOfOrder::Order_Building:
		for (AUnitBase* Unit : UnitsToCommand) { if (Unit) Unit->MoveToBuilding(RaycastHit.GetActor()); }
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, "SassPlayer CommandUnits: Attack a building");
		break;
	case ETypeOfOrder::Order_World:
	default:
		for (AUnitBase* Unit : UnitsToCommand) { if (Unit) Unit->MoveToDest(RaycastHit.Location); }
		break;
	}
}

bool ASassPlayer::CommandUnits_Validate(const TArray<AUnitBase*>& UnitsToCommand, FHitResult RaycastHit,
                                        ETypeOfOrder OrderType)
{
	return true;
}

void ASassPlayer::QuitGame()
{
	FGenericPlatformMisc::RequestExit(false);
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
			PauseWidget->RemoveFromParent();
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
		PauseWidget->RemoveFromParent();
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

void ASassPlayer::ServerCrouch_Implementation(bool bIsCrouching, UCharacterMovementComponent* MovementComponent)
{
	if (bIsCrouching) { MovementComponent->MaxWalkSpeed = CrouchSpeed; }
	else { MovementComponent->MaxWalkSpeed = WalkSpeed; }
}

bool ASassPlayer::ServerCrouch_Validate(bool bIsCrouching, UCharacterMovementComponent* MovementComponent)
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

void ASassPlayer::ServerSprint_Implementation(bool bIsRunning, UCharacterMovementComponent* MovementComponent)
{
	if (bIsRunning) MovementComponent->MaxWalkSpeed = SprintSpeed;
	else MovementComponent->MaxWalkSpeed = WalkSpeed;
}

bool ASassPlayer::ServerSprint_Validate(bool bIsRunning, UCharacterMovementComponent* MovementComponent)
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
	return (PlayerControllerPtr->IsUnitMenuOpen() && IsRightMouseDown);
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
		
		if (Unit && !Hit.GetComponent()->ComponentHasTag(USassCStaticLibrary::NoAggroTag()) && (Unit->GetEmpire()->GetEmpireId() ==
			TempPlayerState->GetEmpire()->GetEmpireId()))
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

void ASassPlayer::GetAllPlayerColors()
{
	TArray<AActor*> FoundControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerControllerClass, FoundControllers);
	for (AActor* FoundController : FoundControllers)
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

void ASassPlayer::LateStart_Implementation(APlayerController* NewPlayer)
{
}

bool ASassPlayer::LateStart_Validate(APlayerController* NewPlayer)
{
	return true;
}


void ASassPlayer::SpawnWallPreview(FVector Location, FRotator Rotation)
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
