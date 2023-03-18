// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once
#include "TypeOfEntity.h"
#include "Core/BuildingRequirements.h"
#include "GameFramework/Info.h"
#include "Empire.generated.h"

class IEntityInterface;
/**
 * 
 */
UCLASS()
class SASSC_API AEmpire : public AInfo
{
	GENERATED_BODY()

public:
	AEmpire();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Empire")
	void InitializeEmpire(const int32 InitEmpireId, const int32 InitPlayerId, const FLinearColor InitEmpireColor);
	
	bool MeetsBuildingRequirement(ETypeOfEntity TypeOfEntity, const FBuildingRequirement& BuildingRequirement, bool bShouldCheckAmount = false);

	static bool IsEmpireValid(const AEmpire* Empire);

	UFUNCTION(BlueprintCallable, Category = "Empire|Buildings")
	void AddEntity(AActor* NewEntity);
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Buildings")
	const TSet<ABuildingBase*>& GetBuildings() const;

	UFUNCTION(BlueprintCallable, Category = "Empire|Units")
	const TSet<AUnitBase*>& GetUnits() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Units")
	const TSet<AUnitBase*>& GetSelectedUnits() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Units")
	int32 NumSelectedUnits() const;

	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	int32 GetGoldIncome() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	int32 GetFoodIncome() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	int32 GetIronIncome() const;

	UFUNCTION(BlueprintCallable, Category = "Empire")
	void CalculateSupply();
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	bool HasColor() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	void SetColor(FColor NewColor);
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	FLinearColor GetColor() const;

	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	void Select();
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	void SelectUnit(AUnitBase* Unit, bool bShouldSelect);
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	void DeselectAllUnits();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	int32 EmpireId = -1;

public:
	int32 GetEmpireId() const;
	void SetEmpireId(const int32 NewEmpireId);

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	int32 OwningPlayerId = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TSet<ABuildingBase*> Buildings = TSet<ABuildingBase*>();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TSet<AUnitBase*> Units = TSet<AUnitBase*>();

	/* Do not use for gameplay purposes */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TMap<ETypeOfEntity, int32> LifetimeSpawns;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TSet<AUnitBase*> SelectedUnits = TSet<AUnitBase*>();
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	FLinearColor EmpireColor = NullColor;
	inline static FColor NullColor = FColor(0, 0, 0, 0);

public:
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void AddGold(int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void AddFood(int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void AddIron(int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void AddSupplyMaximum(int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void AddCreed(int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	float GetFood() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	float GetIron() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	float GetGold() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetCreed() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetSupplyMaximum() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetSupplyUsed() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetNumCities() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetNumShrines() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetNumHouses() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetNumMines() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetNumFarms() const;

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetFood(float NewFood);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetIron(float NewIron);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetGold(float NewGold);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetCreed(int32 NewCreed);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetSupplyMaximum(int32 NewSupplyMaximum);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetSupplyUsed(int32 NewSupplyUsed);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetNumCities(int32 NewNumCities);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetNumShrines(int32 NewNumShrines);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetNumHouses(int32 NewNumHouses);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetNumMines(int32 NewNumMines);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetNumFarms(int32 NewNumFarms);

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	float Food = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	float Iron = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	float Gold = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 Creed = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 SupplyMaximum = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 SupplyUsed = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 NumCities = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 NumShrines = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 NumHouses = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 NumMines = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 NumFarms = 0;
};
