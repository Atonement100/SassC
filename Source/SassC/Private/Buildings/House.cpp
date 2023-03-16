// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE


#include "Buildings/House.h"

#include "TimerManager.h"


// Sets default values
AHouse::AHouse()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AHouse::WhenBuilt()
{
	Super::WhenBuilt();

	// TODO: Spawning in effect

	if (this->BuildingParent)
	{
		FVector ExpansionDirection = (this->GetActorLocation() - this->BuildingParent->GetActorLocation());
		ExpansionDirection.Normalize();
		const FRotator ExpansionRotation = ExpansionDirection.Rotation();

		const FTimerDelegate ExpansionDelegate = FTimerDelegate().CreateLambda([this, ExpansionRotation]
		{
			UE_LOG(Sassilization, Display, TEXT("%s is trying to expand."), *this->GetName())
			this->Expand(ExpansionRotation + FRotator(0, FMath::RandRange(-90.f, 90.f), 0));
		});
		
		FTimerHandle Expansion1, Expansion2, Expansion3;
		GetWorldTimerManager().SetTimer(Expansion1, ExpansionDelegate, 5.f, false); // These are 60, 90, 120
		GetWorldTimerManager().SetTimer(Expansion2, ExpansionDelegate, 10.f, false);
		GetWorldTimerManager().SetTimer(Expansion3, ExpansionDelegate, 15.f, false);	
	}
}

// Called when the game starts or when spawned
void AHouse::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

