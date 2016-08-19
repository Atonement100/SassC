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
	DOREPLIFETIME(AbuildingBase, OwningPlayerColor);
}

void AbuildingBase::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if (BuildingMesh) { BldgMeshMaterialDynamic.Add(BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0))); }
}

void AbuildingBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AbuildingBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AbuildingBase::UpdateMaterial(FLinearColor PlayerColor, bool SetPersistentColor) {
	ColorBldg(PlayerColor, SetPersistentColor);
}

void AbuildingBase::ColorBldg_Implementation(FLinearColor PlayerColor, bool SetPersistentColor) {
	for (UMaterialInstanceDynamic* Material : BldgMeshMaterialDynamic) { if (Material) Material->SetVectorParameterValue(ColorParameterText, PlayerColor); }
	if (SetPersistentColor) { this->OwningPlayerColor = PlayerColor; }
}

bool AbuildingBase::ColorBldg_Validate(FLinearColor PlayerColor, bool SetPersistentColor) {
	return true;
}

float AbuildingBase::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= DamageAmount;
	if (Health <= 0.0f) {
		SetLifeSpan(0.001f);
	}
	return DamageAmount;
}

float AbuildingBase::GetHealth() {
	return Health;
}

void AbuildingBase::FixSpawnLocation_Implementation(FVector RealLocation)
{
	NetFixSpawnLocation(RealLocation);
}

bool AbuildingBase::FixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

void AbuildingBase::HideMesh()
{
}

void AbuildingBase::ShowMesh()
{
}

void AbuildingBase::NetFixSpawnLocation_Implementation(FVector RealLocation)
{
	Role = ROLE_Authority;
	//SetActorLocation(RealLocation + FVector(10));
	SetActorLocation(RealLocation);
}

bool AbuildingBase::NetFixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

#pragma region Upgradeable Building empty fx
void AbuildingBase::PostCreation_Implementation(FLinearColor PlayerColor)
{
}

bool AbuildingBase::PostCreation_Validate(FLinearColor PlayerColor)
{
	return true;
}



void AbuildingBase::PreviewUpgrade_Implementation()
{
}

void AbuildingBase::ResetPreview_Implementation()
{
}

void AbuildingBase::UpgradeBuilding_Implementation()
{
}

bool AbuildingBase::UpgradeBuilding_Validate()
{
	return false;
}

void AbuildingBase::NetUpgradeBuilding_Implementation()
{

}

bool AbuildingBase::NetUpgradeBuilding_Validate()
{
	return false;
}
#pragma endregion