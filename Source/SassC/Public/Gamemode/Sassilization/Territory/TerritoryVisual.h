// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerritoryVisual.generated.h"

class USplineComponent;
class UNiagaraComponent;

UCLASS(Blueprintable, BlueprintType)
class SASSC_API ATerritoryVisual : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATerritoryVisual();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite)
	USplineComponent* SplineComponent;
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* NiagaraSplineEmitter;

	UFUNCTION(Reliable, NetMulticast)
	void MulticastSetSplinePoints(const TArray<AActor*>& LocationActors, FLinearColor SplineColor, int32 RibbonId) const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
