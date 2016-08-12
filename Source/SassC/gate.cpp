// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "gate.h"

// Fill out your copyright notice in the Description page of Project Settings.

Agate::Agate() {
	PrimaryActorTick.bCanEverTick = true;

	BuildingMesh->DestroyComponent();
	BuildingCollision->DestroyComponent();

	GateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gate Mesh"));
	GateMesh->AttachTo(RootComponent);

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Collision"));
	BuildingCollision->AttachTo(RootComponent);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	
	OpenTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Open Trigger"));
	OpenTrigger->AttachTo(RootComponent);
	OpenTrigger->OnComponentBeginOverlap.AddDynamic(this, &Agate::OnOverlapBegin_GateTrigger);
	OpenTrigger->OnComponentEndOverlap.AddDynamic(this, &Agate::OnOverlapEnd_GateTrigger);
}

void Agate::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic = GateMesh->CreateDynamicMaterialInstance(0, GateMesh->GetMaterial(0));

	//FTransform CollisionTransform = BuildingCollision->GetRelativeTransform();
	//CollisionTransform.SetLocation(CollisionDisplacement);

	//BuildingCollision->SetRelativeLocation(CollisionDisplacement);
}

void Agate::BeginPlay() {
	Super::BeginPlay();
}

void Agate::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void Agate::ColorBldg_Implementation(FLinearColor PlayerColor)
{
	BldgMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
	this->GateMesh->SetMaterial(0, BldgMeshMaterialDynamic);
}

bool Agate::ColorBldg_Validate(FLinearColor PlayerColor)
{
	return true;
}

void Agate::OnOverlapBegin_GateTrigger(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Cast<AunitBase>(OtherActor)->OwningPlayerID == this->OwningPlayerID) {
		UnitsInRange++;
	}
}

void Agate::OnOverlapEnd_GateTrigger(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AunitBase>(OtherActor)->OwningPlayerID == this->OwningPlayerID) {
		UnitsInRange--;
	}
}





