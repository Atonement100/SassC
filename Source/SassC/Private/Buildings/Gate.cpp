// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Gate.h"
#include "SassC.h"
#include "Core/SassCStaticLibrary.h"

AGate::AGate()
{
	// Destroy default BuildingBase StaticMesh to replace with Skeletal Mesh
	if (ActiveBuildingMesh) ActiveBuildingMesh->DestroyComponent();  
	ActiveBuildingMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Active Gate Mesh"));
	ActiveBuildingMesh->SetupAttachment(RootComponent);
	ActiveBuildingMesh->SetRelativeLocation(FVector(0, 0, 1));
	ActiveBuildingMesh->SetRelativeScale3D(FVector(USassCStaticLibrary::HammerToUnrealScalar()));

	OpenTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Open Trigger"));
	OpenTrigger->SetupAttachment(RootComponent);
	OpenTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGate::OnOverlapBegin_GateTrigger);
	OpenTrigger->OnComponentEndOverlap.AddDynamic(this, &AGate::OnOverlapEnd_GateTrigger);
	OpenTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3,
	                                           ECollisionResponse::ECR_Ignore);
}

void AGate::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
}

void AGate::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

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
