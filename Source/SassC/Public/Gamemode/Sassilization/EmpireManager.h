// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include <atomic>

#include "Buildings/BuildingBase.h"
#include "Units/UnitBase.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Gamemode/Sassilization/Empire.h"
#include "EmpireManager.generated.h"

USTRUCT()
struct FPlayerAndEmpire
{
	GENERATED_BODY()
	
	int32 PlayerId;
	int32 EmpireId;
	FPlayerAndEmpire()
	{
		PlayerId = -1;
		EmpireId = -1;
	};
	FPlayerAndEmpire(const int32 NewPlayerId, const int32 NewEmpireId) : PlayerId(NewPlayerId), EmpireId(NewEmpireId){}
};

/**
 * 
 */
UCLASS(Blueprintable)
class SASSC_API AEmpireManager : public AActor
{
	GENERATED_BODY()

public:
	AEmpireManager();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	const TArray<AEmpire*>& GetEmpires() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	AEmpire* GetEmpireById(const uint8 EmpireId) const;
	int16 GetEmpireIdForPlayerId(int32 PlayerId) const;

	UFUNCTION(BlueprintCallable, Category = "Empire")
	AEmpire* GetEmpireByPlayerId(const int32 PlayerId) const;

	UFUNCTION(BlueprintCallable, Category = "Empire")
	AEmpire* RetrieveEmpire(const int32 PlayerId, const FString PlayerName);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Empire")
	void ServerCreateNewEmpire(const int32 PlayerId, const FString& PlayerName, ASassPlayerState* SassPlayerState);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Empire")
	void MulticastCreateNewEmpire(const int32 PlayerId, const int32 NewEmpireId, AEmpire* NewEmpire);
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	const FLinearColor& GetColorById(const int ColorId) const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	TArray<ABuildingBase*> GetAllBuildings() const;
	
	UFUNCTION(BlueprintCallable, Category = "Empire")
	TArray<AUnitBase*> GetAllUnits() const;
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Empire")
	TArray<FPlayerAndEmpire> PlayerIdToEmpireId = TArray<FPlayerAndEmpire>();
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Empire")
	TArray<AEmpire*> Empires = TArray<AEmpire*>();
	
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
