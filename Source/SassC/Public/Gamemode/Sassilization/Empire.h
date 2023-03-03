// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once


#include "Empire.generated.h"

/**
 * 
 */
UCLASS()
class SASSC_API UEmpire : public UObject
{
	GENERATED_BODY()

public:
	UEmpire();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Empire")
	void InitializeEmpire(const uint8 InitEmpireId, const int32 InitPlayerId, const FLinearColor InitEmpireColor);

	static bool IsEmpireValid(const UEmpire* Empire);
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	const TSet<ABuildingBase*>& GetBuildings() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	const TSet<AUnitBase*>& GetUnits() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	const TSet<AUnitBase*>& GetSelectedUnits() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
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
	uint8 EmpireId = -1;

public:
	uint8 GetEmpireId() const;
	void SetEmpireId(const uint8 NewEmpireId);

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	int32 OwningPlayerId = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TSet<ABuildingBase*> Buildings = TSet<ABuildingBase*>();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TSet<AUnitBase*> Units = TSet<AUnitBase*>();
	
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
	int32 GetFood() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetIron() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	int32 GetGold() const;
	
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
	void SetFood(int32 NewFood);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetIron(int32 NewIron);

	UFUNCTION(BlueprintCallable, Category = "Empire|Resources")
	void SetGold(int32 NewGold);

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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	int32 Food = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 Iron = 0;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire|Resources")
	int32 Gold = 0;
	
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
