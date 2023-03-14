// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Empire.h"

#include "Buildings/BuildingBase.h"
#include "Units/UnitBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

AEmpire::AEmpire()
{
	this->bReplicates = true;
	this->bAlwaysRelevant = true;
	this->bNetLoadOnClient = true;
}

void AEmpire::InitializeEmpire(const int32 InitEmpireId, const int32 InitPlayerId, const FLinearColor InitEmpireColor)
{
	UE_LOG(Sassilization, Display, TEXT("Initialize new Empire %d, %d, %s"), InitEmpireId, InitPlayerId, *InitEmpireColor.ToString());

	this->EmpireId = InitEmpireId;
	this->OwningPlayerId = InitPlayerId;
	this->EmpireColor = InitEmpireColor;
	
	// todo: extract to gamemode defaults
	this->SetGold(75);
	this->SetFood(100);
	this->SetIron(100);
	this->CalculateSupply();
}

void AEmpire::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEmpire, EmpireId)
	DOREPLIFETIME(AEmpire, OwningPlayerId)
	DOREPLIFETIME(AEmpire, EmpireColor)
	DOREPLIFETIME(AEmpire, Food)
	DOREPLIFETIME(AEmpire, Iron)
	DOREPLIFETIME(AEmpire, Gold)
	DOREPLIFETIME(AEmpire, Creed)
	DOREPLIFETIME(AEmpire, SupplyMaximum)
	DOREPLIFETIME(AEmpire, SupplyUsed)
	DOREPLIFETIME(AEmpire, NumCities)
	DOREPLIFETIME(AEmpire, NumShrines)
	DOREPLIFETIME(AEmpire, NumHouses)
	DOREPLIFETIME(AEmpire, NumMines)
	DOREPLIFETIME(AEmpire, NumFarms)
}

bool AEmpire::IsEmpireValid(const AEmpire* Empire)
{
	return Empire && IsValid(Empire);
}

const TSet<ABuildingBase*>& AEmpire::GetBuildings() const
{
	return this->Buildings;
}

//TODO? Maybe makes sense to just keep a map of these.
bool AEmpire::MeetsBuildingRequirement(ETypeOfEntity TypeOfEntity, const FBuildingRequirement& BuildingRequirement, bool bShouldCheckAmount)
{
	int MatchingCount = 0;
	for (ABuildingBase* Building : this->Buildings)
	{
		const IEntityInterface* Entity = Cast<IEntityInterface>(Building);

		if (Entity->GetTypeOfEntity() == TypeOfEntity && Building->IsBuilt())
		{
			if (BuildingRequirement.ShouldTrackLevel()) 
			{
				if (Building->GetUpgradeLevel() >= BuildingRequirement.LevelRequired)
				{
					return true;
				}
			}
			else if (!BuildingRequirement.ShouldTrackAmount())
			{
				return true;
			}
			else
			{
				MatchingCount += 1;
			}
		}
	}

	if (BuildingRequirement.ShouldTrackAmount())
	{
		return MatchingCount >= BuildingRequirement.AmountRequired;
	}

	return false;
}

void AEmpire::AddEntity(AActor* NewEntity)
{
	const IEntityInterface* NewEntityInterface = Cast<IEntityInterface>(NewEntity);
	
	if (!NewEntityInterface)
	{
		UE_LOG(Sassilization, Warning,
			TEXT("Tried to add entity %s to empire but it was null or does not implement IEntityInterface."),
			*NewEntity->GetName())
		return;
	}
	
	if (NewEntityInterface->IsBuilding())
	{
		this->Buildings.Add(Cast<ABuildingBase>(NewEntity));
	}
	else
	{
		this->Units.Add(Cast<AUnitBase>(NewEntity));
	}

	const ETypeOfEntity NewTypeOfEntity = NewEntityInterface->GetTypeOfEntity();
	const int32 NewCount = LifetimeSpawns.FindOrAdd(NewTypeOfEntity, 0) + 1;
	LifetimeSpawns.Add(NewTypeOfEntity, NewCount);
}

const TSet<AUnitBase*>& AEmpire::GetUnits() const
{
	return this->Units;
}

const TSet<AUnitBase*>& AEmpire::GetSelectedUnits() const
{
	return this->SelectedUnits;
}

int32 AEmpire::NumSelectedUnits() const
{
	return this->SelectedUnits.Num();
}

int32 AEmpire::GetGoldIncome() const
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

int32 AEmpire::GetFoodIncome() const
{
	// todo set the default income food and iron in common area
	return UKismetMathLibrary::FCeil(8 + this->GetNumFarms() * 1.2);
}

int32 AEmpire::GetIronIncome() const
{
	return UKismetMathLibrary::FCeil(8 + this->GetNumFarms());
}

void AEmpire::CalculateSupply()
{
	this->SetSupplyMaximum(UKismetMathLibrary::FFloor(6 + this->GetNumCities() + this->GetNumHouses() * 0.5f));
}

bool AEmpire::HasColor() const
{
	return this->EmpireColor != NullColor;
}

void AEmpire::SetColor(const FColor NewColor)
{
	this->EmpireColor = NewColor;
}

FLinearColor AEmpire::GetColor() const
{
	return this->EmpireColor;
}

void AEmpire::Select()
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

void AEmpire::SelectUnit(AUnitBase* Unit, bool bShouldSelect)
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

void AEmpire::DeselectAllUnits()
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

int32 AEmpire::GetEmpireId() const
{
	return EmpireId;
}

void AEmpire::SetEmpireId(const int32 NewEmpireId)
{
	this->EmpireId = NewEmpireId;
}

void AEmpire::AddGold(const int32 Amount)
{
	int32 NewAmount = this->GetGold() + Amount;
	if (NewAmount < 0)
	{
		NewAmount = 0;
	}

	this->SetGold(NewAmount);
}

void AEmpire::AddFood(const int32 Amount)
{
	this->SetFood(this->GetFood() + Amount);
}

void AEmpire::AddIron(const int32 Amount)
{
	this->SetIron(this->GetIron() + Amount);
}

void AEmpire::AddSupplyMaximum(const int32 Amount)
{
	this->SetCreed(this->GetCreed() + Amount);
}

void AEmpire::AddCreed(const int32 Amount)
{
	this->SetCreed(this->GetCreed() + Amount);
}

float AEmpire::GetFood() const
{
	return Food;
}

float AEmpire::GetIron() const
{
	return Iron;
}

float AEmpire::GetGold() const
{
	return Gold;
}

int32 AEmpire::GetCreed() const
{
	return Creed;
}

int32 AEmpire::GetSupplyMaximum() const
{
	return SupplyMaximum;
}

int32 AEmpire::GetSupplyUsed() const
{
	return SupplyUsed;
}

int32 AEmpire::GetNumCities() const
{
	return NumCities;
}

int32 AEmpire::GetNumShrines() const
{
	return NumShrines;
}

int32 AEmpire::GetNumHouses() const
{
	return NumHouses;
}

int32 AEmpire::GetNumMines() const
{
	return NumMines;
}

int32 AEmpire::GetNumFarms() const
{
	return NumFarms;
}

void AEmpire::SetFood(const float NewFood)
{
	this->Food = NewFood;
}

void AEmpire::SetIron(const float NewIron)
{
	this->Iron = NewIron;
}

void AEmpire::SetGold(const float NewGold)
{
	this->Gold = NewGold;
}

void AEmpire::SetCreed(const int32 NewCreed)
{
	this->Creed = NewCreed;
}

void AEmpire::SetSupplyMaximum(const int32 NewSupplyMaximum)
{
	this->SupplyMaximum = NewSupplyMaximum;
}

void AEmpire::SetSupplyUsed(const int32 NewSupplyUsed)
{
	this->SupplyUsed = NewSupplyUsed;
}

void AEmpire::SetNumCities(const int32 NewNumCities)
{
	this->NumCities = NewNumCities;
}

void AEmpire::SetNumShrines(const int32 NewNumShrines)
{
	this->NumShrines = NewNumShrines;
}

void AEmpire::SetNumHouses(const int32 NewNumHouses)
{
	this->NumHouses = NewNumHouses;
}

void AEmpire::SetNumMines(const int32 NewNumMines)
{
	this->NumMines = NewNumMines;
}

void AEmpire::SetNumFarms(const int32 NewNumFarms)
{
	this->NumFarms = NewNumFarms;
}
