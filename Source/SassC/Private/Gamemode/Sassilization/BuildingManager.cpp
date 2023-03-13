// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/BuildingManager.h"

#include "TimerManager.h"
#include "Buildings/BuildingBase.h"
#include "Buildings/WallSegment.h"
#include "Core/ChannelDefines.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Gamemode/Sassilization/Empire.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Gamemode/Sassilization/Territory/TerritoryManager.h"
#include "Core/BuildingRequirements.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"

bool ABuildingManager::CanAfford(AEmpire* Empire, ETypeOfEntity BuildingType, bool IgnoreCost) const
{
	if (!IsValid(Empire))
	{
		return false;
	}
	
	const TSubclassOf<AActor> ActorToSpawn = GetClassForBuildingType(BuildingType);
	const IEntityInterface* DefaultEntity = Cast<IEntityInterface>(ActorToSpawn.GetDefaultObject());

	if (!ActorToSpawn) return false;

	if (!IgnoreCost)
	{
		const FResourceCosts Costs = DefaultEntity->GetResourceCosts();
		if (Costs.Gold > Empire->GetGold()) return false;
		if (Costs.Food > Empire->GetFood()) return false;
		if (Costs.Iron > Empire->GetIron()) return false;
	}

	FBuildingRequirements BuildingRequirements = DefaultEntity->GetRequirementsForLevel(0);
	for (TPair<ETypeOfEntity, FBuildingRequirement> Requirement : BuildingRequirements.BuildingsRequired)
	{
		if (!Empire->MeetsBuildingRequirement(Requirement.Key, Requirement.Value))
		{
			UE_LOG(LogTemp, Display, TEXT("Couldn't spawn %s because Empire %d did not meet the requirement %s for %s"),
				*UEnum::GetValueAsString(BuildingType), Empire->GetEmpireId(), *Requirement.Value.ToString(), *UEnum::GetValueAsString(Requirement.Key))
			return false;
		}
	}
	
	return true;
}

bool ABuildingManager::AreCornersValid(TArray<FVector> CornerLocations, double& MaxHeight, double& MinHeight) const
{
	if (CornerLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No corner locations were provided"))
		return false;
	}

	FHitResult Hit;
	TArray<float> TraceHeights;
	TArray<AActor*> ActorsToIgnore;

	MaxHeight = -DBL_MAX;
	MinHeight = DBL_MAX;
	
	for (FVector Corner : CornerLocations)
	{
		FVector Start = Corner + FVector(0, 0, 57.15f);
		FVector End = Corner - FVector(0, 0, 19.05f);
		UE_LOG(LogTemp, Display, TEXT("Corner %s Start %s End %s"), *Corner.ToString(), *Start.ToString(), *End.ToString())
		bool bWasHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start,
			End, UEngineTypes::ConvertToTraceType(ECC_LEVEL_MESH),
			true, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true);

		UE_LOG(LogTemp, Display, TEXT("Trace Details: %s"), *Hit.ToString())
		
		if (!bWasHit || Hit.Time == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("Trace at %s had no hit or was inside something (time = 0)"), *Hit.TraceStart.ToString())
			return false;
		}

		if (bWasHit)
		{
			MinHeight = UKismetMathLibrary::Min(MinHeight, Hit.ImpactPoint.Z);
			MaxHeight = UKismetMathLibrary::Max(MaxHeight, Hit.ImpactPoint.Z);
		}
	}
	
	return true;
}

bool ABuildingManager::CanFit(FHitResult PlayerToLocTraceResult, FBox EntityBoundingBox,
	FRotator Rotation, bool bCheckWalls, bool bFoundation) const
{
	if (!PlayerToLocTraceResult.bBlockingHit || PlayerToLocTraceResult.Normal.Z < .7)
	{
		UE_LOG(LogTemp, Display, TEXT("Blocking Hit? %s Normal Z? %f"),
			PlayerToLocTraceResult.bBlockingHit ? TEXT("true") : TEXT("false"), PlayerToLocTraceResult.Normal.Z)
		return false;
	}

	FVector HitLocation = PlayerToLocTraceResult.Location
							- PlayerToLocTraceResult.Normal * EntityBoundingBox.Min.Z
							+ PlayerToLocTraceResult.Normal * .1905;
	FVector BoxCenter = EntityBoundingBox.GetExtent() * 2;
	
	UE_LOG(LogTemp, Display, TEXT("BoundingBox %s"), *EntityBoundingBox.ToString());
	UE_LOG(LogTemp, Display, TEXT("BoxCenter %s"), *BoxCenter.ToString());
	
	TArray<FVector> Corners = {
		FVector(BoxCenter.X, BoxCenter.Y, 0),
		FVector(BoxCenter.X, -BoxCenter.Y, 0),
		FVector(-BoxCenter.X, BoxCenter.Y, 0),
		FVector(-BoxCenter.X, -BoxCenter.Y, 0)
	};

	for (int Index = 0; Index < 4; Index++)
	{
		float Magnitude = Corners[Index].Size2D();
		Corners[Index] = (Corners[Index].Rotation() + Rotation).Vector() * Magnitude + HitLocation;
	}

	UE_LOG(LogTemp, Display, TEXT("Corners: "))
	for (FVector Corner : Corners)
	{
		UE_LOG(LogTemp, Display, TEXT("Corner : %s"), *Corner.ToString())
	}
	
	// TODO: if target location or corners are translucent or underwater return then fail the check
	double MaxHeight, MinHeight;
	bool bAreCornersValid = AreCornersValid(Corners, MaxHeight, MinHeight); 

	if (!bAreCornersValid)
	{
		UE_LOG(LogTemp, Display, TEXT("Corners are not valid."))
		return false;
	}
	
	// Perform box trace, using an adjusted height of +/- .2 hammer units (.381 uu/cm)
	HitLocation.Z = bFoundation ? MaxHeight + .381 : MinHeight - .381;

	FHitResult BoxHit;
	bool bDidBoxHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), HitLocation,
										  HitLocation + FVector(0, 0, EntityBoundingBox.Max.Z * 2),
										  FVector(EntityBoundingBox.Max.X, EntityBoundingBox.Max.Y, 0), Rotation,
										  UEngineTypes::ConvertToTraceType(ECC_SPAWNING), true,
										  TArray<AActor*>(), EDrawDebugTrace::ForDuration, BoxHit, true);
	UE_LOG(LogTemp, Display, TEXT("BoxTrace: %s"), *BoxHit.ToString());
	if (bDidBoxHit)
	{
		if (!BoxHit.GetActor()->IsA(AStaticMeshActor::StaticClass()) || bFoundation)
		{
			return false;
		}
	}
	
	// Check if any wall segments are too close (within 3 units in any direction of the target)
	if (bCheckWalls)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> TraceTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
        TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_SPAWNING));
        TArray<AActor*> HitActors;

		FHitResult DebugHit;
		UKismetSystemLibrary::SphereTraceSingle(GetWorld(), HitLocation, HitLocation, EntityBoundingBox.GetSize().Size(),
			UEngineTypes::ConvertToTraceType(ECC_SPAWNING), true, TArray<AActor*>(), EDrawDebugTrace::ForDuration, DebugHit, true);
        if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), HitLocation, EntityBoundingBox.GetSize().Size(),
        								  TraceTypes, AWallSegment::StaticClass(), TArray<AActor*>(), HitActors))
        {
        	UE_LOG(LogTemp, Display, TEXT("Found %d actors in Wall Check."), HitActors.Num())

        	for (AActor* Actor : HitActors)
        	{
                if (UKismetMathLibrary::IsPointInBox_Box(Actor->GetActorLocation(), FBox(Corners[0], Corners[0] + EntityBoundingBox.Max)))
                {
        	        UE_LOG(LogTemp, Display, TEXT("Actor %s caused failure in wall check at location %s"), *Actor->GetName(), *Actor->GetActorLocation().ToString())
                    return false;
                }
        	}
        }
	}
	
	return true;
}

AActor* ABuildingManager::SpawnGhost(APlayerController* Player, const ETypeOfEntity BuildingToSpawn,
	const FVector Location, const FRotator Rotator) const
{
	if (!Player->IsLocalController())
	{
		return nullptr;
	}
	
	UE_LOG(LogTemp, Display, TEXT("Spawning building %s for player %s at location %s with rotation %s..."),
		*UEnum::GetValueAsString(BuildingToSpawn), *Player->GetPlayerState<ASassPlayerState>()->GetPlayerName(),
		*Location.ToString(), *Rotator.ToString())
	
	const TSubclassOf<AActor> ActorToSpawn = GetClassForBuildingType(BuildingToSpawn);
	
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.Owner = Player;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AActor* NewActor = Cast<AActor>(GetWorld()->SpawnActor(ActorToSpawn, &Location, &Rotator, SpawnParams));
	NewActor->bOnlyRelevantToOwner = true;
	NewActor->SetReplicates(false);
	
	if (!NewActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Building %s failed to spawn for player %s at location %s with rotation %s..."),
			*UEnum::GetValueAsString(BuildingToSpawn), *Player->GetPlayerState<ASassPlayerState>()->GetPlayerName(),
			*Location.ToString(), *Rotator.ToString())
		return nullptr;
	}

	//TODO: Need to spawn with a translucent material.
	NewActor->SetActorEnableCollision(false);
	return NewActor;
}

void ABuildingManager::SpawnBuilding(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector TargetLocation, FRotator Rotator, ATerritoryManager* TerritoryManager)
{
	if (!Player) return;
	
	ASassPlayerState* SassPlayerState = Player->GetPlayerState<ASassPlayerState>();
	if (!SassPlayerState) return;
	if (!SassPlayerState->IsAllowedToSpawnBuilding())
	{
		UE_LOG(LogTemp, Display, TEXT("%s is currently timed out from being able to spawn building."), *SassPlayerState->GetPlayerName())
		return;
	}
	
	SassPlayerState->SetAllowedToSpawnBuilding(false);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
		[SassPlayerState]{SassPlayerState->SetAllowedToSpawnBuilding(true);}), 0.05f, false);
	
	AEmpire* PlayerEmpire = SassPlayerState->GetEmpire();
	if (!PlayerEmpire) return;
	
	const TSubclassOf<AActor> ActorToSpawn = GetClassForBuildingType(BuildingToSpawn);
	const IEntityInterface* DefaultEntity = Cast<IEntityInterface>(ActorToSpawn.GetDefaultObject());

	if (!CanAfford(PlayerEmpire, BuildingToSpawn))
	{
		UE_LOG(LogTemp, Display, TEXT("Player %s tried to spawn %s but couldn't afford it."), *SassPlayerState->GetPlayerName(), *UEnum::GetValueAsString(BuildingToSpawn))
		return;
	}
	
	// Always spawn because we will check the permissibility of a spawn ourselves..
	FTransform TemporaryTransform = FTransform(FVector(-1000, -1000, -1000));
	AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorToSpawn, TemporaryTransform, Player,
		Player->GetPawn(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	FVector Origin, BoxExtent;
	NewActor->GetActorBounds(true, Origin, BoxExtent);
	UE_LOG(LogTemp, Display, TEXT("Box extent %s"), *BoxExtent.ToString())
	FBox BoundingBox = FBox(FVector::Zero(), BoxExtent);
	FHitResult EyeToLocTraceHit;
	FVector EyeLocation;
	FRotator EyeRotation;
	TArray<AActor*> ActorsToIgnore;
	Player->GetPawn()->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	bool bWasHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), EyeLocation,
		TargetLocation + UKismetMathLibrary::GetDirectionUnitVector(EyeLocation, TargetLocation) * 2048.0f,
		UEngineTypes::ConvertToTraceType(ECC_SPAWNING),true, ActorsToIgnore,
		EDrawDebugTrace::ForDuration, EyeToLocTraceHit, true);
	
	if (DefaultEntity->GetTypeOfEntity() == ETypeOfEntity::Gate)
	{
		//TODO: Implement Gate Spawning
	}
	
	if (!CanFit(EyeToLocTraceHit, BoundingBox, Rotator, true, true))
	{
		UE_LOG(LogTemp, Display, TEXT("CanFit check failed."))
		NewActor->Destroy();
		return;
	}
	
	if (DefaultEntity->GetRequirementsForLevel(0).IsEntityRequired(ETypeOfEntity::City))
    {
        if (!TerritoryManager->IsLocationInTerritory(TargetLocation, PlayerEmpire->GetEmpireId()))
        {
            UE_LOG(LogTemp, Display, TEXT("Target Location was not in player's territory but requires a city. Loc %s EmpireId %d"), *TargetLocation.ToString(), PlayerEmpire->GetEmpireId())
        	NewActor->Destroy();
            return;
        }
    }
    else
    {
    	if (!TerritoryManager->IsLocationInTerritory(TargetLocation, 0))
    	{
    		UE_LOG(LogTemp, Display, TEXT("Target Location was not in empty territory but needed to be Loc %s EmpireId %d"), *TargetLocation.ToString(), PlayerEmpire->GetEmpireId())
    		NewActor->Destroy();
    		return;
    	}
    }

	if (DefaultEntity->GetTypeOfEntity() == ETypeOfEntity::Wall)
	{
		TArray<AActor*> NearbyWalls = GetEntitiesOfTypeInSphere(ETypeOfEntity::Wall, TargetLocation, 243.84f);
		for (AActor* WallActor : NearbyWalls)
		{
			if (Cast<IEntityInterface>(WallActor)->GetEmpire() == PlayerEmpire &&
				FVector::Dist(WallActor->GetActorLocation(), TargetLocation) < 60.96f)
			{
				UE_LOG(LogTemp, Display, TEXT("Wall was too close to %s"), *WallActor->GetName())
				NewActor->Destroy();
				return;
			}
		}
	}
	
	//TODO: Implement spawning statistics recording functionality

	PlayerEmpire->AddFood(+DefaultEntity->GetResourceCosts().Food);
	PlayerEmpire->AddIron(+DefaultEntity->GetResourceCosts().Iron);
	PlayerEmpire->AddGold(+DefaultEntity->GetResourceCosts().Gold);

	if (!NewActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor failed to spawn!! Type: %s, Loc: %s, Rot: %s"),
			*UEnum::GetValueAsString(BuildingToSpawn), *TargetLocation.ToString(), *Rotator.ToString())
	}
	
	IEntityInterface* NewEntity = Cast<IEntityInterface>(NewActor);

	FTransform FinalTransform = FTransform(Rotator, TargetLocation);
	NewActor->FinishSpawning(FinalTransform);

	UE_LOG(LogTemp, Display, TEXT("Finished Spawning.. Initializing and adding %s to empire %d"), *NewActor->GetName(), PlayerEmpire->GetEmpireId());
	
	NewEntity->Initialize(PlayerEmpire);
	PlayerEmpire->AddEntity(NewActor);

	//TODO: Factor this into IEntityInterface. Have generic "on spawn" entry point that delegates accordingly, sets up build timer, and so on.
	if (ABuildingBase* Building = Cast<ABuildingBase>(NewActor))
	{
		Building->WhenBuilt();
	}
}

TSubclassOf<AActor> ABuildingManager::GetClassForBuildingType(const ETypeOfEntity TypeOfBuilding) const 
{
	if (!this->BuildingTypeToClass.Contains(TypeOfBuilding))
	{
		UE_LOG(LogTemp, Warning, TEXT("No class configured for building type %s"), *UEnum::GetValueAsString(TypeOfBuilding))
		return nullptr;
	}
	
	return this->BuildingTypeToClass[TypeOfBuilding];
}

TArray<AActor*> ABuildingManager::GetEntitiesOfTypeInSphere(ETypeOfEntity TypeOfEntity, FVector Location, float Radius) const
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Location, Radius,
		TraceObjectTypes, GetClassForBuildingType(TypeOfEntity), TArray<AActor*>(), OverlappingActors);

	return OverlappingActors;
}