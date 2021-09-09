// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetStringLibrary.h"
#include "BuildingBase.h"

ABuildingBase::ABuildingBase()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 20.0f;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneComponent->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = SceneComponent;
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	bReplicates = true;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
	BuildingMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	BuildingMesh->SetRelativeLocation(FVector(0, 0, 1));
	BuildingMesh->SetRelativeScale3D(FVector(1.905));	//1.905 is the Hammer -> UE conversion

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Building Collision"));
	BuildingCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	BuildingCollision->SetRelativeLocation(CollisionDisplacement);

}

void ABuildingBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuildingBase, Health);
	DOREPLIFETIME(ABuildingBase, OwningPlayerID);
	DOREPLIFETIME(ABuildingBase, OwningPlayerColor);
}

void ABuildingBase::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if (BuildingMesh) { BldgMeshMaterialDynamic.Add(BuildingMesh->CreateDynamicMaterialInstance(0, BuildingMesh->GetMaterial(0))); }
}

void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ABuildingBase::UpdateMaterial(FLinearColor PlayerColor, bool SetPersistentColor) {
	ColorBldg(PlayerColor, SetPersistentColor);
}

void ABuildingBase::ColorBldg_Implementation(FLinearColor PlayerColor, bool SetPersistentColor) {
	for (UMaterialInstanceDynamic* Material : BldgMeshMaterialDynamic) { if (Material) Material->SetVectorParameterValue(ColorParameterText, PlayerColor); }
	if (SetPersistentColor) { this->OwningPlayerColor = PlayerColor; }
}

bool ABuildingBase::ColorBldg_Validate(FLinearColor PlayerColor, bool SetPersistentColor) {
	return true;
}

float ABuildingBase::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= DamageAmount;
	if (Health <= 0.0f) {
		SetLifeSpan(0.001f);
	}
	return DamageAmount;
}

float ABuildingBase::GetHealth() {
	return Health;
}

void ABuildingBase::FixSpawnLocation_Implementation(FVector RealLocation)
{
	NetFixSpawnLocation(RealLocation);
}

bool ABuildingBase::FixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

void ABuildingBase::HideMesh()
{
}

void ABuildingBase::ShowMesh()
{
}

UStaticMeshComponent* ABuildingBase::GetMesh(int MeshIndex)
{
	return this->BuildingMesh;
}

void ABuildingBase::NetFixSpawnLocation_Implementation(FVector RealLocation)
{
	SetRole(ROLE_Authority); 
	//This is a horrible thing to do, but spawns are not sent to client with precision beyond int. 
	//I can only pray that the buildings do not rise up and attempt to take over servers. If that happens, we may all be doomed.
	//SetActorLocation(RealLocation + FVector(10));
	SetActorLocation(RealLocation);
}

bool ABuildingBase::NetFixSpawnLocation_Validate(FVector RealLocation)
{
	return true;
}

#pragma region Upgradeable Building empty fx
void ABuildingBase::PostCreation_Implementation(FLinearColor PlayerColor)
{
}

bool ABuildingBase::PostCreation_Validate(FLinearColor PlayerColor)
{
	return true;
}



void ABuildingBase::PreviewUpgrade_Implementation()
{
}

void ABuildingBase::ResetPreview_Implementation()
{
}

void ABuildingBase::UpgradeBuilding_Implementation()
{
}

bool ABuildingBase::UpgradeBuilding_Validate()
{
	return false;
}

void ABuildingBase::NetUpgradeBuilding_Implementation()
{

}

bool ABuildingBase::NetUpgradeBuilding_Validate()
{
	return false;
}
#pragma endregion