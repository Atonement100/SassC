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
	OpenTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
}

void Agate::PostInitializeComponents() {
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic.Add(GateMesh->CreateDynamicMaterialInstance(0, GateMesh->GetMaterial(0))); 
	//This is an override from building base, since it is a skeletal mesh, not a static mesh
	
}

void Agate::BeginPlay() {
	Super::BeginPlay();
}

void Agate::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
/*
void Agate::ColorBldg_Implementation(FLinearColor PlayerColor, int8 MeshLevel)
{
	BldgMeshMaterialDynamic[MeshLevel]->SetVectorParameterValue(ColorParameterText, PlayerColor);
	this->GateMesh->SetMaterial(0, BldgMeshMaterialDynamic[MeshLevel]);
}

bool Agate::ColorBldg_Validate(FLinearColor PlayerColor, int8 MeshLevel)
{
	return true;
}
*/
void Agate::OnOverlapBegin_GateTrigger(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (AunitBase* Unit = Cast<AunitBase>(OtherActor)) {
		if (Unit->OwningPlayerID == this->OwningPlayerID) { 
			UnitsInRange++; 
			ShouldBeOpen = true;
		}
	}
}

void Agate::OnOverlapEnd_GateTrigger(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;
	if (AunitBase* Unit = Cast<AunitBase>(OtherActor)) {
		if (Unit->OwningPlayerID == this->OwningPlayerID) {
			UnitsInRange--;
			if (UnitsInRange == 0) {
				ShouldBeOpen = false;
			}
		}
	}
}
