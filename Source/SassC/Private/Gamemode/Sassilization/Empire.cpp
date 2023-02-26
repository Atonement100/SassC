// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Empire.h"

#include "Units/UnitBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

UEmpire::UEmpire(){}

UEmpire::UEmpire(const uint8 EmpireId, const FString PlayerId, const uint8 ColorId)
{
	UE_LOG(LogTemp, Display, TEXT("Creating new Empire %d, %s, %d"), EmpireId, *PlayerId, ColorId);

	this->EmpireId = EmpireId;
	this->OwningPlayerId = PlayerId;
	this->ColorId = ColorId;
}

void UEmpire::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEmpire, EmpireId)
	DOREPLIFETIME(UEmpire, OwningPlayerId)
	DOREPLIFETIME(UEmpire, ColorId)
	//todo add the rest
}

bool UEmpire::IsEmpireValid(const UEmpire* Empire)
{
	return Empire && IsValid(Empire);
}

const TSet<ABuildingBase*>& UEmpire::GetBuildings() const
{
	return this->Buildings;
}

const TSet<AUnitBase*>& UEmpire::GetUnits() const
{
	return this->Units;
}

const TSet<AUnitBase*>& UEmpire::GetSelectedUnits() const
{
	return this->SelectedUnits;
}

int32 UEmpire::NumSelectedUnits() const
{
	return this->SelectedUnits.Num();
}

int32 UEmpire::GetGoldIncome() const
{
	if (this->GetNumCities() > 0)
	{
		return this->GetNumCities();
	}

	// todo: refactor building data out into some kind of common data object that can be loaded in and shared across the world
	if (this->GetGold() < 32)
	{
		// todo: set this in the gamemode? as a catchup factor
		return 7;
	}

	return 0;
}

int32 UEmpire::GetFoodIncome() const
{
	// todo set the default income food and iron in common area
	return UKismetMathLibrary::FCeil(8 + this->GetNumFarms() * 1.2);
}

int32 UEmpire::GetIronIncome() const
{
	return UKismetMathLibrary::FCeil(8 + this->GetNumFarms());
}

bool UEmpire::HasColor() const
{
	return this->EmpireColor == NullColor;
}

void UEmpire::SetColor(const FColor NewColor)
{
	this->EmpireColor = NewColor;
}

FColor UEmpire::GetColor() const
{
	return this->EmpireColor;
}

void UEmpire::Select()
{
	/* not sure the point and i dont see it used, maybe unneeded
	 *
	 * 
	* function methods:Select(Ent, Select)
	if(Ent) then
		return Ent:Select(Select)
	else
		if(not Select) then
			local unit = self:GetSelectedUnits()[ Ent ]
			if(IsValid(unit)) then
				unit:Select(false)
			end
		end
	end
end
	 */
}

void UEmpire::SelectUnit(AUnitBase* Unit, bool bShouldSelect)
{
	if (bShouldSelect)
	{
		this->SelectedUnits.Add(Unit);
	}
	else
	{
		this->SelectedUnits.Remove(Unit);
	}
}

void UEmpire::DeselectAllUnits()
{
	for (AUnitBase* Unit : this->SelectedUnits)
	{
		if (Unit)
		{
			Unit->SelectUnit(false);
		}
	}

	this->SelectedUnits.Empty();
}

void UEmpire::AddGold(const int32 Amount)
{
	int32 NewAmount = this->GetGold() + Amount;
	if (NewAmount < 0)
	{
		NewAmount = 0;
	}

	this->SetGold(NewAmount);
}

void UEmpire::AddFood(const int32 Amount)
{
	this->SetFood(this->GetFood() + Amount);
}

void UEmpire::AddIron(const int32 Amount)
{
	this->SetIron(this->GetIron() + Amount);
}

void UEmpire::AddSupplyMaximum(const int32 Amount)
{
	this->SetCreed(this->GetCreed() + Amount);
}

void UEmpire::AddCreed(const int32 Amount)
{
	this->SetCreed(this->GetCreed() + Amount);
}

int32 UEmpire::GetFood() const
{
	return Food;
}

int32 UEmpire::GetIron() const
{
	return Iron;
}

int32 UEmpire::GetGold() const
{
	return Gold;
}

int32 UEmpire::GetCreed() const
{
	return Creed;
}

int32 UEmpire::GetSupplyMaximum() const
{
	return SupplyMaximum;
}

int32 UEmpire::GetSupplyUsed() const
{
	return SupplyUsed;
}

int32 UEmpire::GetNumCities() const
{
	return NumCities;
}

int32 UEmpire::GetNumShrines() const
{
	return NumShrines;
}

int32 UEmpire::GetNumHouses() const
{
	return NumHouses;
}

int32 UEmpire::GetNumMines() const
{
	return NumMines;
}

int32 UEmpire::GetNumFarms() const
{
	return NumFarms;
}

void UEmpire::SetFood(const int32 NewFood)
{
	this->Food = NewFood;
}

void UEmpire::SetIron(const int32 NewIron)
{
	this->Iron = NewIron;
}

void UEmpire::SetGold(const int32 NewGold)
{
	this->Gold = NewGold;
}

void UEmpire::SetCreed(const int32 NewCreed)
{
	this->Creed = NewCreed;
}

void UEmpire::SetSupplyMaximum(const int32 NewSupplyMaximum)
{
	this->SupplyMaximum = NewSupplyMaximum;
}

void UEmpire::SetSupplyUsed(const int32 NewSupplyUsed)
{
	this->SupplyUsed = NewSupplyUsed;
}

void UEmpire::SetNumCities(const int32 NewNumCities)
{
	this->NumCities = NewNumCities;
}

void UEmpire::SetNumShrines(const int32 NewNumShrines)
{
	this->NumShrines = NewNumShrines;
}

void UEmpire::SetNumHouses(const int32 NewNumHouses)
{
	this->NumHouses = NewNumHouses;
}

void UEmpire::SetNumMines(const int32 NewNumMines)
{
	this->NumMines = NewNumMines;
}

void UEmpire::SetNumFarms(const int32 NewNumFarms)
{
	this->NumFarms = NewNumFarms;
}


