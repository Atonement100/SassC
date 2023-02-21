// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Buildings/Gate.h"

AGate::AGate()
{
	BuildingMesh->DestroyComponent();
	BuildingCollision->DestroyComponent();

	GateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gate Mesh"));
	GateMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Collision"));
	BuildingCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	BuildingCollision->SetBoxExtent(CollisionBounds);

	OpenTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Open Trigger"));
	OpenTrigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	OpenTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGate::OnOverlapBegin_GateTrigger);
	OpenTrigger->OnComponentEndOverlap.AddDynamic(this, &AGate::OnOverlapEnd_GateTrigger);
	OpenTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3,
	                                           ECollisionResponse::ECR_Ignore);
}

void AGate::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BldgMeshMaterialDynamic.Add(GateMesh->CreateDynamicMaterialInstance(0, GateMesh->GetMaterial(0)));
	//This is an override from building base, since it is a skeletal mesh, not a static mesh
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
}

void AGate::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
void AGate::OnOverlapBegin_GateTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (AUnitBase* Unit = Cast<AUnitBase>(OtherActor))
	{
		if (Unit->OwningPlayerID == this->OwningPlayerID)
		{
			UnitsInRange++;
			ShouldBeOpen = true;
		}
	}
}

void AGate::OnOverlapEnd_GateTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;
	if (AUnitBase* Unit = Cast<AUnitBase>(OtherActor))
	{
		if (Unit->OwningPlayerID == this->OwningPlayerID)
		{
			UnitsInRange--;
			if (UnitsInRange == 0)
			{
				ShouldBeOpen = false;
			}
		}
	}
}
