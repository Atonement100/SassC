// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/EmpireManager.h"

const TMap<uint8, UEmpire*>& AEmpireManager::GetEmpires() const
{
	return this->Empires;
}

UEmpire* AEmpireManager::GetEmpireById(const uint8 EmpireId) const
{
	return *(this->Empires.Find(EmpireId));
}

UEmpire* AEmpireManager::GetEmpireByPlayerId(const FString PlayerId) const
{
	const uint8 EmpireId = *(this->PlayerIdToEmpireId.Find(PlayerId));
	return this->GetEmpireById(EmpireId);
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
