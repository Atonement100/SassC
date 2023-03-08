// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/BuildingManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Gamemode/Sassilization/Empire.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"

bool ABuildingManager::CanBuild(AEmpire* Empire, EBuildingType BuildingType, bool IgnoreCost) const
{
	if (!IsValid(Empire))
	{
		return false;
	}

	// todo check building attributes and cost, need to factor that out of building base itself to do it though

	// todo check building requirements
	
	return true;
}

AActor* ABuildingManager::SpawnGhost(APlayerController* Player, const ETypeOfEntity BuildingToSpawn,
	const FVector Location, const FRotator Rotator)
{
	UE_LOG(LogTemp, Display, TEXT("Spawning building %s for player %s at location %s with rotation %s..."),
		*UEnum::GetValueAsString(BuildingToSpawn), *Player->GetPlayerState<ASassPlayerState>()->GetPlayerName(),
		*Location.ToString(), *Rotator.ToString())
	
	const TSubclassOf<AActor> ActorToSpawn = GetClassForBuildingType(BuildingToSpawn);
	
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.Owner = Player;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AActor* NewActor = Cast<AActor>(GetWorld()->SpawnActor(ActorToSpawn, &Location, &Rotator, SpawnParams));

	if (!NewActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Building %s failed to spawn for player %s at location %s with rotation %s..."),
			*UEnum::GetValueAsString(BuildingToSpawn), *Player->GetPlayerState<ASassPlayerState>()->GetPlayerName(),
			*Location.ToString(), *Rotator.ToString())
		return nullptr;
	}

	//todo make this ghost material
	NewActor->SetActorEnableCollision(false);
	return NewActor;
}

void ABuildingManager::SpawnBuilding(APlayerController* Player, ETypeOfEntity BuildingToSpawn, FVector Location, FRotator Rotator)
{
	// ActorToSpawn->GetDefaultObject(true);
	const TSubclassOf<AActor> ActorToSpawn = GetClassForBuildingType(BuildingToSpawn);
	
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	GetWorld()->SpawnActor(ActorToSpawn, &Location, &Rotator, SpawnParams);
}

TSubclassOf<AActor> ABuildingManager::GetClassForBuildingType(const ETypeOfEntity TypeOfBuilding)
{
	if (!this->BuildingTypeToClass.Contains(TypeOfBuilding))
	{
		UE_LOG(LogTemp, Warning, TEXT("No class configured for building type %s"), *UEnum::GetValueAsString(TypeOfBuilding))
		return nullptr;
	}
	
	return this->BuildingTypeToClass[TypeOfBuilding];
}
