// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include <atomic>

#include "Buildings/BuildingBase.h"
#include "Units/UnitBase.h"
#include "Empire.h"
#include "EmpireManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API AEmpireManager : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Empire")
	const TMap<uint8, UEmpire*>& GetEmpires() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	UEmpire* GetEmpireById(const uint8 EmpireId) const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	UEmpire* GetEmpireByPlayerId(const int32 PlayerId) const;

	UFUNCTION(BlueprintCallable, Category = "Empire")
	UEmpire* RetrieveOrCreateNewEmpire(const int32 PlayerId, const FString PlayerName);
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	const FLinearColor& GetColorById(const int ColorId) const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	TArray<ABuildingBase*> GetAllBuildings() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	TArray<AUnitBase*> GetAllUnits() const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TMap<int32, uint8> PlayerIdToEmpireId = TMap<int32, uint8>();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TMap<uint8, UEmpire*> Empires = TMap<uint8, UEmpire*>();
	
	std::atomic_int EmpireIdCounter = std::atomic_int(1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Empire")
	TArray<FLinearColor> AvailableColors = TArray<FLinearColor>(std::initializer_list<FLinearColor>({
		FColorList::Red,
		FColorList::Turquoise, 
		FColorList::Yellow, 
		FColorList::Pink,
		FColorList::Blue, 
		FColorList::Brown, 
		FColorList::Green, 
		FColorList::DarkPurple,
		FColorList::DarkOliveGreen,
		FColorList::Grey, 
		FColorList::Orange,
		FColorList::LimeGreen,
		FColorList::SpicyPink,
		FColorList::Maroon,	
		FColorList::YellowGreen,
		FColorList::Magenta,
		FColorList::Yellow,
		FColorList::Cyan,
		FColorList::Orange,
		FColorList::BlueViolet,
		FColorList::Brown,
	}));
};
