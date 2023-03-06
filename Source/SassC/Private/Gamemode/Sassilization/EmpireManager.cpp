// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/EmpireManager.h"

#include "Net/UnrealNetwork.h"

AEmpireManager::AEmpireManager()
{
	this->bReplicates = true;
}

void AEmpireManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEmpireManager, Empires)
	DOREPLIFETIME(AEmpireManager, PlayerIdToEmpireId)
}

const TArray<AEmpire*>& AEmpireManager::GetEmpires() const
{
	return this->Empires;
}

AEmpire* AEmpireManager::GetEmpireById(const uint8 EmpireId) const
{
	for (AEmpire* const Empire : Empires)
	{
		if (Empire && Empire->GetEmpireId() == EmpireId)
		{
			return Empire;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Empire not found for EmpireId %d"), EmpireId)
	return nullptr;
}

int16 AEmpireManager::GetEmpireIdForPlayerId(const int32 PlayerId) const
{
	int16 EmpireId = -1;

	for (const FPlayerAndEmpire PlayerAndEmpire : PlayerIdToEmpireId)
	{
		if (PlayerAndEmpire.PlayerId == PlayerId)
		{
			EmpireId = PlayerAndEmpire.EmpireId;
			break;
		}
	}

	return EmpireId;
}

AEmpire* AEmpireManager::GetEmpireByPlayerId(const int32 PlayerId) const
{
	const int16 EmpireId = GetEmpireIdForPlayerId(PlayerId);
	
	if (EmpireId == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("EmpireId not found for PlayerId %d"), PlayerId)
		return nullptr;
	}

	return this->GetEmpireById(EmpireId);
}

AEmpire* AEmpireManager::RetrieveEmpire(const int32 PlayerId, const FString PlayerName)
{
	if (AEmpire* PotentialEmpire = GetEmpireByPlayerId(PlayerId))
	{
		UE_LOG(LogTemp, Display, TEXT("EmpireId %d already exists for player %s with id %d"), PotentialEmpire->GetEmpireId(), *PlayerName, PlayerId);
		return PotentialEmpire;
	}

	UE_LOG(LogTemp, Warning, TEXT("Empire was not able to be retrieved.."))
	return nullptr;
	// ServerCreateNewEmpire(PlayerId, PlayerName);
	//
	// AEmpire* NewEmpire = GetEmpireByPlayerId(PlayerId);
	//
	// if (!NewEmpire)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Failed to create a new empire for player %s with id %d"), *PlayerName, PlayerId);
	// }
	
	// return NewEmpire;

}

void AEmpireManager::ServerCreateNewEmpire_Implementation(const int32 PlayerId, const FString& PlayerName, ASassPlayerState* SassPlayerState)
{
	const int NewEmpireId = EmpireIdCounter.fetch_add(1);
	UE_LOG(LogTemp, Display, TEXT("Creating a new empire for %s with empireId %d. Player's UniqueId is %d"), *PlayerName, NewEmpireId, PlayerId);

	AEmpire* NewEmpire = GetWorld()->SpawnActor<AEmpire>(AEmpire::StaticClass());
	NewEmpire->InitializeEmpire(NewEmpireId, PlayerId, GetColorById(NewEmpireId));
	NewEmpire->AddFood(NewEmpireId);
	Empires.Add(NewEmpire);
	PlayerIdToEmpireId.Add(FPlayerAndEmpire(PlayerId, NewEmpireId));

	SassPlayerState->SetEmpire(NewEmpire);
	
	MulticastCreateNewEmpire(PlayerId, NewEmpireId, NewEmpire);
}

bool AEmpireManager::ServerCreateNewEmpire_Validate(const int32 PlayerId, const FString& PlayerName, ASassPlayerState* SassPlayerState)
{
	return true;
}

void AEmpireManager::MulticastCreateNewEmpire_Implementation(const int32 PlayerId, const int32 NewEmpireId,
	AEmpire* NewEmpire)
{
	if (GetEmpireIdForPlayerId(PlayerId) != NewEmpireId)
	{
		PlayerIdToEmpireId.Add(FPlayerAndEmpire(PlayerId, NewEmpireId));
	}

	if (!GetEmpireById(NewEmpireId))
	{
		Empires.Add(NewEmpire);
	}
}

bool AEmpireManager::MulticastCreateNewEmpire_Validate(const int32 PlayerId, const int32 NewEmpireId,
	AEmpire* NewEmpire)
{
	return true;
}

const FLinearColor& AEmpireManager::GetColorById(const int ColorId) const
{
	if (!this->AvailableColors.IsValidIndex(ColorId))
	{
		UE_LOG(LogTemp, Warning, TEXT("ColorId Not Valid %d"), ColorId);
	}

	return this->AvailableColors[ColorId];
}

TArray<ABuildingBase*> AEmpireManager::GetAllBuildings() const
{
	TArray<ABuildingBase*> AllBuildings = TArray<ABuildingBase*>();
	
	for (const AEmpire* Empire : this->Empires)
	{
		for (ABuildingBase* const Building : Empire->GetBuildings())
		{
			if (!IsValid(Building))
			
			AllBuildings.Add(Building);
		}
	}

	return AllBuildings;
}

TArray<AUnitBase*> AEmpireManager::GetAllUnits() const
{
	TArray<AUnitBase*> AllUnits = TArray<AUnitBase*>();
	
	for (AEmpire* const Empire : this->Empires)
	{
		for (AUnitBase* const Unit : Empire->GetUnits())
		{
			if (!IsValid(Unit))
			
				AllUnits.Add(Unit);
		}
	}

	return AllUnits;
}
