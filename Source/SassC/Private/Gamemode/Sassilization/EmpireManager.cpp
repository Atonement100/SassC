// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/EmpireManager.h"

const TMap<uint8, UEmpire*>& AEmpireManager::GetEmpires() const
{
	return this->Empires;
}

UEmpire* AEmpireManager::GetEmpireById(const uint8 EmpireId) const
{
	UEmpire* const* FoundEmpire = this->Empires.Find(EmpireId);

	if (!FoundEmpire)
	{
		UE_LOG(LogTemp, Warning, TEXT("Empire not found for EmpireId %d"), EmpireId)
		return nullptr;
	}

	return *FoundEmpire;
}

UEmpire* AEmpireManager::GetEmpireByPlayerId(const int32 PlayerId) const
{
	const uint8* EmpireId = this->PlayerIdToEmpireId.Find(PlayerId);
	
	if (!EmpireId)
	{
		UE_LOG(LogTemp, Warning, TEXT("EmpireId not found for PlayerId %d"), PlayerId)
		return nullptr;
	}
	
	return this->GetEmpireById(*EmpireId);
}

UEmpire* AEmpireManager::RetrieveOrCreateNewEmpire(const int32 PlayerId, const FString PlayerName)
{
	if (UEmpire* PotentialEmpire = GetEmpireByPlayerId(PlayerId))
	{
		UE_LOG(LogTemp, Display, TEXT("EmpireId %d already exists for player %s with id %d"), PotentialEmpire->GetEmpireId(), *PlayerName, PlayerId);
		return PotentialEmpire;
	}

	const int NewEmpireId = EmpireIdCounter.fetch_add(1);
	UE_LOG(LogTemp, Display, TEXT("Creating a new empire for %s with empireId %d. Player's UniqueId is %d"), *PlayerName, NewEmpireId, PlayerId);
	
	UEmpire* NewEmpire = NewObject<UEmpire>();
	NewEmpire->InitializeEmpire(NewEmpireId, PlayerId, GetColorById(NewEmpireId));

	Empires.Add(NewEmpireId, NewEmpire);
	PlayerIdToEmpireId.Add(PlayerId, NewEmpireId);
	
	return NewEmpire;
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
	
	for (const TPair<uint8, UEmpire*>& EmpirePair : this->Empires)
	{
		const UEmpire* Empire = EmpirePair.Value;
		
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
	
	for (const TPair<uint8, UEmpire*>& EmpirePair : this->Empires)
	{
		const UEmpire* Empire = EmpirePair.Value;
		
		for (AUnitBase* const Unit : Empire->GetUnits())
		{
			if (!IsValid(Unit))
			
				AllUnits.Add(Unit);
		}
	}

	return AllUnits;
}
