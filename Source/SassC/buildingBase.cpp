// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetStringLibrary.h"
#include "buildingBase.h"

AbuildingBase::AbuildingBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneComponent->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = SceneComponent;
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	bReplicates = true;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
	BuildingMesh->AttachTo(RootComponent);
	BuildingMesh->SetRelativeLocation(FVector(0, 0, 1));
	BuildingMesh->SetRelativeScale3D(FVector(1.905));

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Building Collision"));
	BuildingCollision->AttachTo(RootComponent);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	BuildingCollision->SetRelativeLocation(CollisionDisplacement);

}

void AbuildingBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AbuildingBase, Health);
	DOREPLIFETIME(AbuildingBase, OwningPlayerID);
}

void AbuildingBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AbuildingBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AbuildingBase::UpdateMaterial(FLinearColor PlayerColor) {
	ColorBldg(PlayerColor);
}

void AbuildingBase::ColorBldg_Implementation(FLinearColor PlayerColor) {
	BldgMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
	this->BuildingMesh->SetMaterial(0, BldgMeshMaterialDynamic);
}

bool AbuildingBase::ColorBldg_Validate(FLinearColor PlayerColor) {
	return true;
}

float AbuildingBase::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.0f) {
		SetLifeSpan(0.001f);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, UKismetStringLibrary::Conv_FloatToString(Health));
	}
	return DamageAmount;
}

float AbuildingBase::GetHealth() {
	return Health;
}

void AbuildingBase::PostCreation_Implementation(FLinearColor PlayerColor)
{
}

bool AbuildingBase::PostCreation_Validate(FLinearColor PlayerColor)
{
	return true;
}
