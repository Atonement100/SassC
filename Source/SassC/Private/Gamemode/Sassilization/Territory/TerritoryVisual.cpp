// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Gamemode/Sassilization/Territory/TerritoryVisual.h"
#include "NiagaraComponent.h"
#include "Components/SplineComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATerritoryVisual::ATerritoryVisual()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SetRootComponent(SplineComponent);
	
	NiagaraSplineEmitter = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSplineEmitter"));
	NiagaraSplineEmitter->SetupAttachment(SplineComponent);
}

void ATerritoryVisual::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATerritoryVisual::MulticastSetSplinePoints_Implementation(const TArray<AActor*>& LocationActors,
	FLinearColor SplineColor, int32 RibbonId) const
{
	this->SplineComponent->ClearSplinePoints(false);
	this->NiagaraSplineEmitter->SetNiagaraVariableObject("InputSpline", this->SplineComponent);
	this->NiagaraSplineEmitter->SetNiagaraVariableInt("RibbonID", RibbonId);
	this->NiagaraSplineEmitter->SetNiagaraVariableInt("NumParticles", LocationActors.Num());
	this->NiagaraSplineEmitter->SetNiagaraVariableLinearColor("SplineColor", SplineColor);
	
	for (int Index = 0; Index < LocationActors.Num(); Index++)
	{
		const AActor* Actor = LocationActors[Index];
		this->SplineComponent->AddSplineWorldPoint(Actor->GetActorLocation() + FVector(0, 0, .1));
		this->SplineComponent->SetSplinePointType(Index, ESplinePointType::Linear, false);
	}
	
	this->SplineComponent->UpdateSpline();
	this->SplineComponent->Deactivate();

	UE_LOG(Sassilization, Display, TEXT("Spline has %d points"), this->SplineComponent->GetNumberOfSplinePoints())
}

// Called when the game starts or when spawned
void ATerritoryVisual::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerritoryVisual::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

