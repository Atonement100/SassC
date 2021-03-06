	// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "UnrealNetwork.h"
#include "sassPlayer.h"
#include "sassPauseMenu.h"
#include "sassPlayerController.h"
#include "sassPlayerState.h"
#include "sassGameState.h"
#include "unitBase.h"
#include "city.h"
#include "wall.h"
#include "gate.h"
#include "wallSegment.h"
#include "shieldMonolith.h"
#include "tower.h"
#include "workshop.h"
#include "selectionSphere.h"
#include "buildingBase.h"
#include "unitController.h"
#include "SassCStaticLibrary.h"
#include "ParticleDefinitions.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h" //necessary only for debugging

#define MAX_BLDG_CORNER_DIFFERENCE 6.0f


AsassPlayer::AsassPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	TempGateWalls.SetNumZeroed(2);	// This is done to give TempGateWalls an exact size, so that we can reference [0..1]
									// Gate walls are potentially a very frequent check, so this aids in cutting down unnecessary repetitive checks
}

void AsassPlayer::BeginPlay()
{
	Super::BeginPlay();
	PlayerControllerPtr = UGameplayStatics::GetPlayerController(this, 0);
	CleanupHUD();

	AsassGameState* SassGameStateRef = Cast<AsassGameState>(GetWorld()->GetGameState());
	if (!SassGameStateRef->PreGameActive) {
		CreateGameHUD();
	}
	else {
		CreatePregameHUD();
	}
}

void AsassPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AsassPlayer, SelectedSpawnableClass);
}

void AsassPlayer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (IsUnitMenuOpen) {
		FHitResult CursorHit;

		//@TODO: Find a better solution for this. Perhaps custom constructor taking in bNoCollisionFail value.
		FActorSpawnParameters TempParams = FActorSpawnParameters();
		TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);

		if (LocalObjectSpawn != nullptr && !UKismetMathLibrary::EqualEqual_ClassClass(SelectedSpawnableClass, LocalObjectClass)) {
			LocalObjectSpawn->Destroy();
			ActorSpawnLatch = true;
		}
		
		if (ActorSpawnLatch) {
			if (PlayerControllerPtr) PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, CursorHit);
			FTransform Transform = FTransform(PreviewRotation, CursorHit.Location, FVector(1));

			LocalObjectSpawn = GetWorld()->SpawnActor(SelectedSpawnableClass, &Transform, SpawnParams);
			if (LocalObjectSpawn != nullptr) {
				LocalObjectSpawn->SetOwner(PlayerControllerPtr);	//I don't know if this line actually works, honestly
				LocalObjectSpawn->SetActorEnableCollision(false);
			}
			else { GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "SassPlayer Tick LocalObject didn't spawn"); }
			LocalObjectClass = SelectedSpawnableClass;
			if (SelectionSphereHolder) SelectionSphereHolder->Destroy();
			SphereSpawnLatch = true;
			ActorDestroyLatch = true;
			ActorSpawnLatch = false;
		}
		
		if (PlayerControllerPtr != nullptr) PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, CursorHit);
		if (LocalObjectSpawn != nullptr) {
			if (!IsRightMouseDown) { 
				LocalObjectSpawn->SetActorLocation(CursorHit.Location + ((SelectedSpawnableType < ETypeOfSpawnable::UNIT_SOLDIER)? FVector::ZeroVector : CurrentHalfHeight)); 
			}
			else if (IsRightMouseDown) {
				FHitResult RotateHit;
				TArray<AActor*> RaycastIgnores;
				UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) + UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation())*10000.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::None, RotateHit, true);
				PreviewRotation = FRotator(0,(RotateHit.Location - LocalObjectSpawn->GetActorLocation()).Rotation().Yaw,0) - FRotator(0,90,0);
				LocalObjectSpawn->SetActorRotation(PreviewRotation);
			}
		}
		
		if (CursorHit.GetActor() && !CursorHit.GetActor()->IsA(AbuildingBase::StaticClass())) {			//Trace hit something that isn't a building
			if (ResetLocalView) { LocalObjectSpawn->SetActorHiddenInGame(false); ResetLocalView = false; }
			if (CursorHit.Normal.Z > .990) {
				//@TODO: HalfHeight and TraceSize will only ever change when spawnable changes. Instead of checking this on tick, check it when spawnable is switched.
				FVector HalfHeight = CurrentHalfHeight, TraceSize = CurrentTraceSize;
				

				FHitResult BoxTraceHit;
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(LocalObjectSpawn);
				IsBadSpawn = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), CursorHit.Location + FVector(0, 0, 2), CursorHit.Location + 2 * CurrentHalfHeight, FVector(CurrentTraceSize.X, CurrentTraceSize.Y, 0), PreviewRotation, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), true, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, BoxTraceHit, true);
				if (AbuildingBase* BuildingCast = Cast<AbuildingBase>(LocalObjectSpawn)) {
					IsBadSpawn = IsBadSpawn | CheckBldgCorners(BuildingCast->CornerLocations, CursorHit.Location, PreviewRotation, PlayerState->PlayerId, (Cast<Acity>(LocalObjectSpawn) ? true : false));
				}
				else if (AunitBase* UnitCast = Cast<AunitBase>(LocalObjectSpawn)) {
					IsBadSpawn = IsBadSpawn | CheckUnitLocation(CursorHit.Location, PreviewRotation, PlayerState->PlayerId);
				}

			}
			else { 
				IsBadSpawn = true; 
				for (Awall* Wall : WallsBeingPreviewed) {
					if (Wall->TempConnection) {
						Wall->TempConnection->Destroy();
						Wall->TempConnection = nullptr;
					}
				}
				WallsBeingPreviewed.Empty();
			}

			FLinearColor NewColor = IsBadSpawn ? FLinearColor(.7f, 0.0f, .058f) : FLinearColor(.093f, .59f, .153f);
			
			if (AbuildingBase* BuildingCast = Cast<AbuildingBase>(LocalObjectSpawn)) { BuildingCast->UpdateMaterial(NewColor, false); }
			else if (AunitBase* UnitCast = Cast<AunitBase>(LocalObjectSpawn)) { UnitCast->UpdateMaterial(NewColor); }

			if (Agate* GateCast = Cast <Agate>(LocalObjectSpawn)) {
				ResetGatePreviewLatch = true;
				const FTransform LeftTransform = FTransform(FRotator::ZeroRotator, LocalObjectSpawn->GetActorLocation() + (PreviewRotation + FRotator(0, 90, 0)).Vector() * 50);
				const FTransform RightTransform = FTransform(FRotator::ZeroRotator, LocalObjectSpawn->GetActorLocation() + (PreviewRotation + FRotator(0, 90, 0)).Vector() * -50);

				if (TempGateWalls[0] != nullptr) TempGateWalls[0]->SetActorTransform(LeftTransform);
				else if (!TempGateWalls[0]) { 
					TempGateWalls[0] = GetWorld()->SpawnActor(WallClass, &LeftTransform, SpawnParams);
					Cast<Awall>(TempGateWalls[0])->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
				}
				if (TempGateWalls[1] != nullptr) TempGateWalls[1]->SetActorTransform(RightTransform);
				else if (!TempGateWalls[1]) { 
					TempGateWalls[1] = GetWorld()->SpawnActor(WallClass, &RightTransform, SpawnParams); 
					Cast<Awall>(TempGateWalls[1])->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
				}

				TempGateWallsBeingPreviewed.Empty();
				for (AActor* Wall : TempGateWalls) {
					if (!Wall) continue;
					Awall* WallInstance = Cast<Awall>(Wall);
					Awall* TargetWall = WallInstance->GetClosestWallTowerInRange(100.0f, TempGateWalls);
					if (!TargetWall) {
						if (WallInstance->TempConnection) {
							WallInstance->TempConnection->Destroy();
							WallInstance->TempConnection = nullptr;
						}
						continue;
					}
					TempGateWallsBeingTargeted.Add(TargetWall);
					FVector Displacement = TargetWall->GetActorLocation() - Wall->GetActorLocation();
					FVector UnitDirection = Displacement / Displacement.Size();
					FHitResult Hit;
					if (TargetWall->OwningPlayerID == PlayerState->PlayerId && !UKismetSystemLibrary::BoxTraceSingle(GetWorld(), WallInstance->GetActorLocation() + FVector(UnitDirection.X * 24, UnitDirection.Y * 24, 21), TargetWall->GetActorLocation() + FVector(UnitDirection.X * -24, UnitDirection.Y * -24, 21), FVector(9.5f, 4.0f, 15.0f), Displacement.Rotation(), UEngineTypes::ConvertToTraceType(ECC_Visibility), true, TempGateWalls, EDrawDebugTrace::ForOneFrame, Hit, true)) {
						if (!WallInstance->TempConnection) {
							//create new
							FTransform Transform = FTransform(Displacement.Rotation(), WallInstance->GetActorLocation() + (Displacement / 2), FVector(Displacement.Size() / 19, 1, 1));
							WallInstance->TempConnection = GetWorld()->SpawnActor(WallSegmentGhostClass, &Transform, SpawnParams);
						}
						else if (WallInstance->TempConnection) {
							//update existing
							FVector Displacement = TargetWall->GetActorLocation() - WallInstance->GetActorLocation();
							FTransform Transform = FTransform(Displacement.Rotation(), WallInstance->GetActorLocation() + (Displacement / 2), FVector(Displacement.Size() / 19, 1, 1));
							WallInstance->TempConnection->SetActorTransform(Transform);
						}
					}
					else {
						if (WallInstance->TempConnection) {
							WallInstance->TempConnection->Destroy();
							WallInstance->TempConnection = nullptr;
						}
					}
				}
		
			}
			else if (ResetGatePreviewLatch) {
				ResetGatePreviewLatch = false;
				for (int Index = 0; Index < TempGateWalls.Num(); Index++) {
					if (!TempGateWalls[Index]) continue;
					if (Awall* WallRefCast = Cast<Awall>(TempGateWalls[Index])) {
						if (WallRefCast->TempConnection) {
							WallRefCast->TempConnection->Destroy();
							WallRefCast->TempConnection = nullptr;
						}
					}
					TempGateWalls[Index]->Destroy();
					TempGateWalls[Index] = nullptr;
				}
			}

			if (Awall* WallCast = Cast<Awall>(LocalObjectSpawn)) {
				ResetWallPreviewLatch = true;
				WallPreviewArray = (WallCast->FindWallTowersInRange());
				FHitResult Hit;
				TArray<AActor*> ActorsToIgnore;
				for (Awall* TargetWall : WallPreviewArray) {
					//if not yet previewed, create a preview and add to existing preview array, need to keep track of connected walls and their order
					FVector Displacement = TargetWall->GetActorLocation() - WallCast->GetActorLocation();
					FVector UnitDirection = Displacement / Displacement.Size();
					if (TargetWall->OwningPlayerID == PlayerState->PlayerId && !UKismetSystemLibrary::BoxTraceSingle(GetWorld(), WallCast->GetActorLocation() + FVector(UnitDirection.X*24, UnitDirection.Y*24, 21), TargetWall->GetActorLocation() + FVector(UnitDirection.X * -24, UnitDirection.Y * -24, 21), FVector(9.5f, 4.0f, 15.0f), Displacement.Rotation(), UEngineTypes::ConvertToTraceType(ECC_Visibility), true, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, Hit, true)) {
						if (!TargetWall->TempConnection) {
							//create new
							FTransform Transform = FTransform(Displacement.Rotation(), WallCast->GetActorLocation() + (Displacement / 2), FVector(Displacement.Size() / 19, 1, 1));
							TargetWall->TempConnection = GetWorld()->SpawnActor(WallSegmentGhostClass, &Transform, SpawnParams);
							WallsBeingPreviewed.Add(TargetWall);

						}
						else if (TargetWall->TempConnection) {
							//update existing
							FVector Displacement = TargetWall->GetActorLocation() - WallCast->GetActorLocation();
							FTransform Transform = FTransform(Displacement.Rotation(), WallCast->GetActorLocation() + (Displacement / 2), FVector(Displacement.Size() / 19, 1, 1));
							TargetWall->TempConnection->SetActorTransform(Transform);
						}
					}
					else {
						if (TargetWall->TempConnection) {
							TargetWall->TempConnection->Destroy();
							TargetWall->TempConnection = nullptr;
						}
					}
				}	

				for (Awall* WallToCheck : WallsBeingPreviewed) {
					if (!WallPreviewArray.Contains(WallToCheck) && WallToCheck->TempConnection) {
						WallToCheck->TempConnection->Destroy();
						WallToCheck->TempConnection = nullptr;
						//WallsBeingPreviewed.Remove(WallToCheck);
					}
				}
			}
			else if (ResetWallPreviewLatch) {
				for (Awall* Wall : WallsBeingPreviewed) {
					if (Wall->TempConnection) {
						Wall->TempConnection->Destroy();
						Wall->TempConnection = nullptr;
					}
				}
				WallsBeingPreviewed.Empty();
				ResetWallPreviewLatch = false;
			}

		}
		else if (CursorHit.GetActor()) {	//Did hit something and it was a building
			if (ResetGatePreviewLatch) {
				ResetGatePreviewLatch = false;
				for (int Index = 0; Index < TempGateWalls.Num(); Index++) {
					if (!TempGateWalls[Index]) continue;
					if (Awall* WallRefCast = Cast<Awall>(TempGateWalls[Index])) {
						if (WallRefCast->TempConnection) {
							WallRefCast->TempConnection->Destroy();
							WallRefCast->TempConnection = nullptr;
						}
					}
					TempGateWalls[Index]->Destroy();
					TempGateWalls[Index] = nullptr;
				}
			}

			 for (Awall* Wall : WallsBeingPreviewed) {
				 if (Wall->TempConnection) {
					 Wall->TempConnection->Destroy();
					 Wall->TempConnection = nullptr;
				 }
			 }
			 WallsBeingPreviewed.Empty();

			if (LocalObjectSpawn->IsA(Atower::StaticClass()) && CursorHit.GetActor()->IsA(Atower::StaticClass())) {
				LocalObjectSpawn->SetActorHiddenInGame(true);
				Atower* TempTower = Cast<Atower>(CursorHit.GetActor());
				if (TempTower->OwningPlayerID == PlayerState->PlayerId) TempTower->PreviewActive = true;
				ResetLocalView = true;
			}
			else if (LocalObjectSpawn->IsA(Aworkshop::StaticClass()) && CursorHit.GetActor()->IsA(Aworkshop::StaticClass())) {
				LocalObjectSpawn->SetActorHiddenInGame(true);
				Aworkshop* TempWorkshop = Cast<Aworkshop>(CursorHit.GetActor());
				if (TempWorkshop->OwningPlayerID == PlayerState->PlayerId) TempWorkshop->PreviewActive = true;
				ResetLocalView = true;
			}
		}
	}
	//Unit Menu not open
	else {
		if (ActorDestroyLatch) { //Do not combine into one if statement, still want to reset latch if object is nullptr
			if (LocalObjectSpawn) LocalObjectSpawn->Destroy(); 
			for (int Index = 0; Index < TempGateWalls.Num(); Index++) {
				if (!TempGateWalls[Index]) continue;
				if (Awall* WallRefCast = Cast<Awall>(TempGateWalls[Index])) {
					if (WallRefCast->TempConnection) {
						WallRefCast->TempConnection->Destroy();
						WallRefCast->TempConnection = nullptr;
					}
				}
				TempGateWalls[Index]->Destroy();
				TempGateWalls[Index] = nullptr;
			}

			if (WallsBeingPreviewed.Num() != 0) {
				for (Awall* Wall : WallsBeingPreviewed) {
					if (Wall->TempConnection) {
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
		if (IsLeftMouseDown) {
			const TArray<AActor*> RaycastIgnore;
			if (SphereSpawnLatch) {
				FHitResult InitRaycastHit;
				const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
				UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) + UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation())*10000.0f, StaticObjectTypes, true, RaycastIgnore, EDrawDebugTrace::ForOneFrame, InitRaycastHit, true);
				InitialHit = InitRaycastHit.Location;
				FTransform Transform = FTransform(FRotator::ZeroRotator, InitialHit, FVector(1));
				SelectionSphereHolder = (AselectionSphere*)(GetWorld()->SpawnActor(SelectionSphereClass, &Transform, SpawnParams));
				if(!ShouldAddToSelection) TurnOffAllSelectionCircles();
				SphereDestroyLatch = true;
				ActorSpawnLatch = true;
				SphereSpawnLatch = false;
			}
			FHitResult RaycastHit;
			UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) + UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation())*10000.0f, StaticObjectTypes, true, RaycastIgnore, EDrawDebugTrace::ForOneFrame, RaycastHit, true);
			CurrentHit = RaycastHit.Location;
			SphereTraceRadius = (CurrentHit - InitialHit).Size()/2; //i.e. get half length of spanning vector as the radius
			FVector SphereCenter = FVector((CurrentHit.X + InitialHit.X) / 2, (CurrentHit.Y + InitialHit.Y) / 2, (CurrentHit.Z + InitialHit.Z) / 2);
			UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), SphereCenter, SphereCenter + FVector(0, 0, 10), SphereTraceRadius, DynamicObjectTypes, false, RaycastIgnore, EDrawDebugTrace::ForOneFrame, SphereTraceHits, true);
			if (SelectionSphereHolder)SelectionSphereHolder->SetActorTransform(FTransform(FRotator::ZeroRotator, SphereCenter, FVector(SphereTraceRadius / SelectionSphereScaleMod)));

		}
		//Left Mouse button not pressed
		else if (SphereDestroyLatch) {
			if (SelectionSphereHolder) SelectionSphereHolder->Destroy();
			CreateSelectedUnitsArray(SphereTraceHits, PlayerState->PlayerId, ShouldAddToSelection);
			ActorSpawnLatch = true;
			SphereSpawnLatch = true;
			SphereDestroyLatch = false;
		}
	}
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
	FHitResult Hit;
	if (!PlayerControllerPtr) return;
	PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit); // Assign Hit
	if (!Hit.GetActor()) return;
	FVector Location = Hit.GetActor()->GetActorLocation();
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, UKismetStringLibrary::Conv_VectorToString(Location));
}


void AsassPlayer::LeftClickPressed() {
	IsLeftMouseDown = true;
	if (!IsRightMouseDown) {
		if (IsUnitMenuOpen) {
			if (PlayerControllerPtr == nullptr) return;
			FHitResult Hit;
			TArray<FVector> LocationsToCheck;
			TArray<AActor*> ActorsToIgnore = TArray<AActor*>();

			PlayerControllerPtr->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit); // Assign Hit

			if (AbuildingBase* LocalBuildingRef = Cast<AbuildingBase>(LocalObjectSpawn)) {
				LocationsToCheck = LocalBuildingRef->CornerLocations;
				if (SelectedSpawnableType == ETypeOfSpawnable::BUILDING_GATE) { ActorsToIgnore = TempGateWalls; }
			}

			const TArray<AActor*> ConfirmedToIgnore = ActorsToIgnore;
			ServerSpawnBuilding(Cast<AsassPlayerController>(PlayerControllerPtr), SelectedSpawnableClass, Hit, PreviewRotation, CurrentHalfHeight, LocationsToCheck, CurrentTraceSize, PlayerState->PlayerId, ConfirmedToIgnore);
		}
	}
}

void AsassPlayer::LeftClickReleased() {
	IsLeftMouseDown = false;
}

void AsassPlayer::RightClickPressed() {
	IsRightMouseDown = true;

	FHitResult RaycastHit;
	const TArray<AActor*> RaycastIgnores;

	if (!IsUnitMenuOpen) {
		UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) + UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation())*10000.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::ForDuration, RaycastHit, true);
		if (RaycastHit.GetComponent()->ComponentHasTag(USassCStaticLibrary::NoAggroTag())) { UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f), GetMesh()->GetComponentLocation() + FVector(0, 0, BaseEyeHeight + 80.0f) + UKismetMathLibrary::GetForwardVector(PlayerControllerPtr->GetControlRotation())*10000.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, RaycastIgnores, EDrawDebugTrace::ForDuration, RaycastHit, true); }
		AActor* HitActor = RaycastHit.GetActor();
		ETypeOfOrder OrderType = ETypeOfOrder::ORDER_WORLD;
		if ((HitActor->IsA(AunitBase::StaticClass()) || HitActor->IsA(AselectionSphere::StaticClass())) && Cast<AunitBase>(HitActor)->OwningPlayerID != PlayerState->PlayerId) { OrderType = ETypeOfOrder::ORDER_UNIT; }
		if (HitActor->IsA(AbuildingBase::StaticClass()) && Cast<AbuildingBase>(HitActor)->OwningPlayerID != PlayerState->PlayerId) { OrderType = ETypeOfOrder::ORDER_BUILDING; }
		CommandUnits(SelectedUnits, RaycastHit, OrderType);
	}
}

void AsassPlayer::RightClickReleased() {
	IsRightMouseDown = false;
}

void AsassPlayer::CommandUnits_Implementation(const TArray<AunitBase*> &SelectedUnits, FHitResult RaycastHit, ETypeOfOrder OrderType) {
	switch (OrderType) {
	case ETypeOfOrder::ORDER_UNIT:
		for (AunitBase* Unit : SelectedUnits) { if(Unit) Unit->MoveToUnit(RaycastHit.GetActor()); }
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, "SassPlayer CommandUnits: Attack a unit");
		break;
	case ETypeOfOrder::ORDER_BUILDING:
		for (AunitBase* Unit : SelectedUnits) { if(Unit) Unit->MoveToBuilding(RaycastHit.GetActor()); }
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, "SassPlayer CommandUnits: Attack a building");
		break;
	case ETypeOfOrder::ORDER_WORLD:
	default:
		for (AunitBase* Unit : SelectedUnits) { if(Unit) Unit->MoveToDest(RaycastHit.Location); }
		break;
	}
}

bool AsassPlayer::CommandUnits_Validate(const TArray<AunitBase*> &SelectedUnits, FHitResult RaycastHit, ETypeOfOrder OrderType) {
	return true;
}

#pragma region HUD functions
void AsassPlayer::UnitMenuPressed() {
	if (!IsUnitMenuOpen) {
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->bShowMouseCursor = true;
			PlayerControllerPtr->SetInputMode(FInputModeGameAndUI());
			OpenUnitMenu();
			UpdateSelectedSpawnableClass(SelectedSpawnableClass, SelectedSpawnableType);
		}
		IsUnitMenuOpen = true;
	}
	else {
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->bShowMouseCursor = false;
			PlayerControllerPtr->SetInputMode(FInputModeGameOnly());
			CloseUnitMenu();
		}
		IsUnitMenuOpen = false;
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
	if (PlayerControllerPtr->IsLocalController()) {
		RemoveAllWidgets();
		GameWidget = CreateWidget<UUserWidget>(PlayerControllerPtr, GameWidgetClass);
		if (GameWidget != nullptr) GameWidget->AddToViewport();
		PlayerControllerPtr->SetInputMode(FInputModeGameOnly());
		PlayerControllerPtr->bShowMouseCursor = false;
		SetSassHUDRef();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "SassPlayer Creategamehud - local controller not found");
	}
}

UUserWidget* AsassPlayer::GetGameWidget() {
	return GameWidget;
}
#pragma endregion

void AsassPlayer::QuitGame() {
	FGenericPlatformMisc::RequestExit(false);
}

void AsassPlayer::UpdateSelectedSpawnableClass(UClass * NewClass, ETypeOfSpawnable NewTypeOfSpawnable)
{
	if (!NewClass) return;
	SelectedSpawnableClass = NewClass;
	SelectedSpawnableType = NewTypeOfSpawnable;
	UObject* NewActor = SelectedSpawnableClass->GetDefaultObject();
	if (NewActor->IsA(AbuildingBase::StaticClass())) {
		AbuildingBase* SelectedBuilding = Cast<AbuildingBase>(NewActor);
		CurrentHalfHeight = SelectedBuilding->HalfHeight;
		CurrentTraceSize = SelectedBuilding->TraceSize;
	}
	else if(NewActor->IsA(AunitBase::StaticClass())) {
		AunitBase* SelectedUnit = Cast<AunitBase>(NewActor);
		CurrentHalfHeight = FVector(0,0,SelectedUnit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		CurrentTraceSize = SelectedUnit->TraceSize;
	}
}

#pragma region Pause functions
void AsassPlayer::PausePressed() {
	
	if (!IsPaused) {
		if (PauseWidgetClass != nullptr) PauseWidget = CreateWidget<UsassPauseMenu>(PlayerControllerPtr, PauseWidgetClass);
		if (PauseWidget != nullptr) PauseWidget->AddToViewport();
		if (PlayerControllerPtr != nullptr) {
			PlayerControllerPtr->SetInputMode(FInputModeGameAndUI());
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
			PlayerControllerPtr->SetInputMode(FInputModeGameOnly());
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
	BaseEyeHeight = CrouchingEyeHeight;
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	ServerCrouch(true, GetCharacterMovement());
}

void AsassPlayer::CrouchReleased() {
	BaseEyeHeight = StandingEyeHeight;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ServerCrouch(false, GetCharacterMovement());
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
	ShouldAddToSelection = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	ServerSprint(true, GetCharacterMovement());
}

void AsassPlayer::SprintReleased() {
	ShouldAddToSelection = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ServerSprint(false, GetCharacterMovement());
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
		FRotator Rotation = GetMesh()->GetComponentRotation() - GetMesh()->RelativeRotation;
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

bool AsassPlayer::ShouldIgnoreLookInput()
{
	return (IsUnitMenuOpen && IsRightMouseDown);
}
#pragma endregion

#pragma region Unit Interaction
void AsassPlayer::TurnOffAllSelectionCircles()
{
	if (PlayerState == nullptr) { return; }
	for (AActor* UnitIt : (((AsassPlayerState*)PlayerState)->ControlledBuildings)) {
		AunitBase* Unit = Cast<AunitBase>(UnitIt);
		if (Unit != nullptr) { Unit->SetDecalVisibility(false); }
	}
}

void AsassPlayer::CreateSelectedUnitsArray(TArray<FHitResult> Hits, int32 PlayerID, bool AddToSelection)
{
	if (!AddToSelection) { 
		TurnOffAllSelectionCircles(); 
		SelectedUnits.Empty(); 
	}
	AsassPlayerState* TempPlayerState = ((AsassPlayerState*)PlayerState);
	
	if (TempPlayerState == nullptr) { return; } //Shouldn't happen

	for (FHitResult Hit : Hits) {
		AunitBase* Unit = Cast<AunitBase>(Hit.GetActor());


		if (Unit && !Hit.GetComponent()->ComponentHasTag(USassCStaticLibrary::NoAggroTag()) && (Unit->OwningPlayerID == PlayerID)) {
			SelectedUnits.Add(Unit);
			Unit->SetDecalVisibility(true);
		}
	}

	TempPlayerState->SelectedUnits = SelectedUnits;
}

AActor* AsassPlayer::GetSelectionSphereHolder() {
	return SelectionSphereHolder;
}

#pragma endregion

#pragma region Server-side Spawning
void AsassPlayer::ServerSpawnBuilding_Implementation(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, FRotator Rotator, const FVector &HalfHeight, const TArray<FVector> &Midpoints, const FVector &TraceSize, int32 PlayerID, const TArray<AActor*> &ActorsToIgnore)
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters WallParams = FActorSpawnParameters(TempParams);

	const FVector Location = Hit.Location + HalfHeight;
	const FRotator Rotation = Rotator;

	bool SpawningBuilding = ActorToSpawn.GetDefaultObject()->IsA(AbuildingBase::StaticClass());

	if (Hit.GetActor() && !Hit.GetActor()->IsA(AbuildingBase::StaticClass())) {
		if (Hit.Normal.Z >= .990) {
			const TArray<AActor*> BoxIgnore;
			FHitResult BoxHit;

			if (!UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Hit.Location + FVector(0, 0, 2), Hit.Location + 2 * HalfHeight, FVector(TraceSize.X, TraceSize.Y, 0), Rotation, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), true, ActorsToIgnore, EDrawDebugTrace::ForDuration, BoxHit, true)) {
				GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, UKismetStringLibrary::Conv_VectorToString(Location));
				//if there is no hit (good)
				if ((ActorToSpawn.GetDefaultObject()->IsA(AbuildingBase::StaticClass()) && !CheckBldgCorners(Midpoints, Hit.Location, PreviewRotation, PlayerID, ActorToSpawn.GetDefaultObject()->IsA(Acity::StaticClass())))
					|| ((ActorToSpawn.GetDefaultObject()->IsA(AunitBase::StaticClass())) && !CheckUnitLocation(Hit.Location, PreviewRotation, PlayerID))) {
					//if there is no obstruction (good)
					AActor* NewSpawn = GetWorld()->SpawnActor(ActorToSpawn, &Location, &Rotation, SpawnParams);

					if (NewSpawn == nullptr) {
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "SassPlayer ServerSpawnBuilding: Could not spawn, unknown reason");
						return;
					}

					if (SpawningBuilding) { NewSpawn->SetActorLocation(Hit.Location); }
					AsassPlayerState* SassPlayerState = Cast<AsassPlayerState>(PlayerState);
					if (SassPlayerState != nullptr) {
						SassPlayerState->ControlledBuildings.Add(NewSpawn);
						NewSpawn->SetOwner(PlayerController);
					}

					//@TODO: Should find a way to consolidate these... hard with units being ACharacter, buildings being AActor to give them a common unit
					//try unit (more likely)
					if (AunitBase* NewUnit = Cast<AunitBase>(NewSpawn)) {
						NewUnit->UpdateMaterial(SassPlayerState->PlayerColor);
						NewUnit->SpawnDefaultController();
						NewUnit->OwningPlayerID = PlayerID;

						//Don't think units need to have location fixed. Small change shouldn't matter. If it does an alternative solution will need to be found, as this causes issues.
						//NewUnit->FixSpawnLocation(Location);
					}
					//try building
					else {
						if (AbuildingBase* NewBuilding = Cast<AbuildingBase>(NewSpawn)) {
							NewBuilding->UpdateMaterial(SassPlayerState->PlayerColor, true);
							NewBuilding->OwningPlayerID = PlayerID;
							NewBuilding->PostCreation(SassPlayerState->PlayerColor);
							NewBuilding->FixSpawnLocation(Hit.Location);
							if (Awall* WallCast = Cast<Awall>(NewBuilding)) {						
								TArray<Awall*> WallsInRange = WallCast->FindWallTowersInRange();
								for (Awall* TargetWall : WallsInRange) {
									if (TargetWall->OwningPlayerID == PlayerID) { ServerSpawnWall(WallCast, TargetWall, PlayerID, SassPlayerState->PlayerColor); }
								}
							}
							else if (Agate* GateCast = Cast<Agate>(NewBuilding)) {
								//@TODO Check if I can skip AActor* and just spawn in with Ignore.Add(Getworld. . .  .
								const FVector LeftLoc = Hit.Location + HalfHeight + (Rotator + FRotator(0, 90, 0)).Vector() * 50 + FVector(0, 0, -20);
								const FVector RightLoc = Hit.Location + HalfHeight + (Rotator + FRotator(0, 90, 0)).Vector() * -50 + FVector(0, 0, -20);
								AActor* x = GetWorld()->SpawnActor(WallClass, &LeftLoc, &Rotation, WallParams);
								AActor* y = GetWorld()->SpawnActor(WallClass, &RightLoc, &Rotation, WallParams);
								TArray<AActor*> Ignore = TArray<AActor*>(ActorsToIgnore);
								Ignore.Add(x);
								Ignore.Add(y);

								for (int SideNum = 0; SideNum < Ignore.Num(); SideNum++) {
									if (!Ignore[SideNum]) continue;
									Awall* Wall = Cast<Awall>(Ignore[SideNum]);
									Wall->OwningPlayerID = PlayerID;
									Wall->UpdateMaterial(SassPlayerState->PlayerColor, true);
									Wall->PostCreation(SassPlayerState->PlayerColor);
									ServerSpawnWall(Wall, Wall->GetClosestWallTowerInRange(100.0f, Ignore), PlayerID, SassPlayerState->PlayerColor);
								}
							}
							else if (AshieldMonolith* MonoCast = Cast<AshieldMonolith>(NewBuilding)) {
								TArray<AshieldMonolith*> MonolithsInRange = MonoCast->FindMonolithsInRange();
								if (MonolithsInRange.Num() > 0) {
									UParticleSystem* PSysToSpawn = MonoCast->BeamPSys;
									for (AshieldMonolith* TargetMonolith : MonolithsInRange) { TargetMonolith->SpawnBeamEmitter(PSysToSpawn, MonoCast, TargetMonolith); }
								}
							}
						}
						else { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "SassPlayer ServerSpawnBuilding: Could not spawn, server could not determine what spawn was being asked for"); }
					}
				}
				else {
					//Bad Spawn
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "SassPlayer ServerSpawnBuilding: Could not spawn because corners were obstructed");
				}
			}
			else {
				//Bad Spawn
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "SassPlayer ServerSpawnBuilding: Could not spawn because trace hit something");
			}
		}
		else {
			//Bad Spawn
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "SassPlayer ServerSpawnBuilding: Could not spawn because location uneven");
		}
	}
	else if (Hit.GetActor()) {
		if (ActorToSpawn.GetDefaultObject()->IsA(Atower::StaticClass()) && Hit.GetActor()->IsA(Atower::StaticClass())) {
			Atower* TempTower = Cast<Atower>(Hit.GetActor());
			if (TempTower->OwningPlayerID == PlayerID) TempTower->UpgradeBuilding();
		}
		else if (ActorToSpawn.GetDefaultObject()->IsA(Aworkshop::StaticClass()) && Hit.GetActor()->IsA(Aworkshop::StaticClass())) {
			Aworkshop* TempWorkshop = Cast<Aworkshop>(Hit.GetActor());
			if (TempWorkshop->OwningPlayerID == PlayerID) TempWorkshop->UpgradeBuilding();
		}
	}
}

bool AsassPlayer::ServerSpawnBuilding_Validate(AsassPlayerController* PlayerController, TSubclassOf<AActor> ActorToSpawn, FHitResult Hit, FRotator Rotator, const FVector &HalfHeight, const TArray<FVector> &Midpoints, const FVector &TraceSize, int32 PlayerID, const TArray<AActor*> &ActorsToIgnore)
{
	return true;
}
#pragma endregion

void AsassPlayer::GetAllPlayerColors() {
	TArray<AActor*> FoundControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerControllerClass, FoundControllers);
	for (AActor* Controller : FoundControllers) {

	}
}

void AsassPlayer::ColorPlayer_Implementation(FLinearColor PlayerColor)
{
	if (DynamicPlayerMaterial == nullptr || GetMesh() == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "PlayerMaterial or Mesh bad");
		return;
	}
	DynamicPlayerMaterial->SetVectorParameterValue(ColorParameterName, PlayerColor);
	GetMesh()->SetMaterial(0, DynamicPlayerMaterial);
}

bool AsassPlayer::ColorPlayer_Validate(FLinearColor PlayerColor)
{
	return true;
}

bool AsassPlayer::CheckUnitLocation(FVector Center, FRotator Rotator, int32 PlayerID) {
	TArray<AActor*> nullArray;
	FHitResult Hit;
	if (UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), Center + FVector(0, 0, 55.0f), Center - FVector(0, 0, 15.0f), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), false, nullArray, EDrawDebugTrace::ForOneFrame, Hit, true)) {
		if (AbuildingBase* Bldg = Cast<AbuildingBase>(Hit.GetActor())) {
			if (Bldg->OwningPlayerID == PlayerID) {
				GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald, "SassPlayer CheckUnitLocation: good spawn inside TERRITORY");
				return false;
			}
		}
	}
	return true;
}

bool AsassPlayer::CheckBldgCorners(TArray<FVector> ExtraLocs, FVector Center, FRotator Rotator, int32 PlayerID, bool isCity)
{
	if (ExtraLocs.Num() == 0){ GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No extre locs"); return true; }

	FHitResult Hit;
	FVector DirectionUnitVector = (Rotator).Vector();
	TArray<float> TraceHeights;
	TArray<AActor*> Ignore;

	for (FVector Loc : ExtraLocs) {	
		float Magnitude = Loc.Size2D();
		FVector Displacement = (Loc.Rotation() + Rotator).Vector()*Magnitude;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Center + Displacement + FVector(0,0,15.0f), Center + Displacement - FVector(0,0,15.0f), StaticObjectTypes, true, Ignore, EDrawDebugTrace::ForOneFrame, Hit, true)) {
			TraceHeights.Add(Hit.Location.Z);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald, "SassPlayer CheckBldgCorners: TRACE MADE NO CONTACT");
			return true;
		}
	}
	float first = TraceHeights[0];
	for (float Height : TraceHeights) {
		if (FMath::Abs(Height - first) > MAX_BLDG_CORNER_DIFFERENCE) {
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald, "SassPlayer CheckBldgCorners: HEIGHTS NOT EQUAL");
			return true;
		}
	}


	TArray<AActor*> nullArray;
	for (FVector Loc : ExtraLocs) {
		float Magnitude = Loc.Size2D();
		FVector Displacement = (Loc.Rotation() + Rotator).Vector()*Magnitude;
		if (UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), Center + Displacement + FVector(0, 0, 65.0f), Center + Displacement - FVector(0, 0, 15.0f), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), false, nullArray, EDrawDebugTrace::ForOneFrame, Hit, true)){
			if (AbuildingBase* Bldg = Cast<AbuildingBase>(Hit.GetActor())) {
				if (Bldg->OwningPlayerID != PlayerID) {
					GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Emerald, "SassPlayer CheckBldgCorners: OVERLAPS ENEMY TERRITORY");
					return true;
				}
			}
		}
		else if (!isCity) { //Trace did not hit
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Green, "SassPlayer CheckBldgCorners: TERRITORY TRACE DID NOT MAKE CONTACT");
			return true;
		}
	}

	if (isCity) {
		TArray<FHitResult> SphereHits;
		UKismetSystemLibrary::SphereTraceMulti_NEW(GetWorld(), Center, Center + FVector(0,0,1), USassCStaticLibrary::CityDefaultInfluenceRange(), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), false, nullArray, EDrawDebugTrace::ForOneFrame, SphereHits, true);
		for (FHitResult AHit : SphereHits) {
			if (AbuildingBase* Bldg = Cast<AbuildingBase>(AHit.GetActor())) {
				if (Bldg->OwningPlayerID != PlayerID) {
					GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Orange, "SassPlayer CheckBldgCorners: CITY TERRITORY OVERLAPS ENEMY TERRITORY");
					return true;
				}
			}
		}
	}

	return false;
}

void AsassPlayer::LateStart_Implementation(APlayerController* NewPlayer)
{

}

bool AsassPlayer::LateStart_Validate(APlayerController* NewPlayer)
{
	return true;
}


void AsassPlayer::SpawnWallPreview_Implementation(FVector Location, FRotator Rotation)
{	
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	GetWorld()->SpawnActor(WallSegmentClass, &Location, &Rotation, SpawnParams);

}

void AsassPlayer::ServerSpawnWall_Implementation(Awall* NewWall, Awall* TargetWall, int32 PlayerID, FLinearColor PlayerColor)
{
	if (!TargetWall) {GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "ServerSpawnWall: TargetWall does not exist"); return;}
	FActorSpawnParameters TempParams = FActorSpawnParameters();
	TempParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters(TempParams);
	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;

	FVector Direction = NewWall->GetActorLocation() - TargetWall->GetActorLocation();
	FVector UnitDirection = Direction / Direction.Size();

	if (!UKismetSystemLibrary::BoxTraceSingle(GetWorld(), NewWall->GetActorLocation() + FVector(UnitDirection.X*-24, UnitDirection.Y*-24, 21), TargetWall->GetActorLocation() + FVector(UnitDirection.X * 24, UnitDirection.Y * 24, 21), FVector(9.5f, 4.0f, 15.0f), Direction.Rotation(), UEngineTypes::ConvertToTraceType(ECC_Visibility), true, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true)) {
		float SpaceBetween = ((TargetWall->GetActorLocation() - FVector(Direction.X / Direction.Size2D() * -24, Direction.Y / Direction.Size2D() * -24, 0)) - (NewWall->GetActorLocation() + FVector(Direction.X / Direction.Size2D() * 24, Direction.Y / Direction.Size2D() * 24, 0))).Size();
		FVector Start = NewWall->GetActorLocation() + FVector(UnitDirection.X * -18, UnitDirection.Y * -18, 0);
		bool FirstLoop = true, LastLoop = false;
		AwallSegment* PreviousSegment = nullptr;
		for (int NumToSpawn = (SpaceBetween / 19); NumToSpawn > 0; NumToSpawn--) {
			const FVector Loc = Start;
			const FRotator Rot = Direction.Rotation();
			AwallSegment* NewSegment = Cast<AwallSegment>(GetWorld()->SpawnActor(WallSegmentClass, &Loc, &Rot, SpawnParams));
			NewSegment->OwningPlayerID = PlayerID;
			NewSegment->UpdateMaterial(PlayerColor, true);
			NewSegment->FixSpawnLocation(Loc);
			if (FirstLoop) {
				NewWall->AddConnectedWallSegment(NewSegment);
				NewSegment->LeftConnection = NewWall;
				FirstLoop = false;
			}
			else if (LastLoop) {
				PreviousSegment->RightConnection = NewSegment;
				NewSegment->LeftConnection = PreviousSegment;
				NewSegment->RightConnection = TargetWall;
				TargetWall->AddConnectedWallSegment(NewSegment);
			}
			else {
				NewSegment->LeftConnection = PreviousSegment;
				PreviousSegment->RightConnection = NewSegment;
			}
			if (NumToSpawn == 2) { LastLoop = true; }

			PreviousSegment = NewSegment;
			Start = Start + FVector(UnitDirection.X * -19, UnitDirection.Y * -19, 0);
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "ServerSpawnWall: Spawn was blocked");

	}
}

bool AsassPlayer::ServerSpawnWall_Validate(Awall* NewWall, Awall* TargetWall, int32 PlayerID, FLinearColor PlayerColor)
{
	return true;
}

#pragma region Blueprint Implementables
void AsassPlayer::SetSassHUDRef_Implementation()
{
}

void AsassPlayer::OpenUnitMenu_Implementation()
{
}

void AsassPlayer::CloseUnitMenu_Implementation()
{
}
#pragma endregion