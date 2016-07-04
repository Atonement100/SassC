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
	
}

void AbuildingBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AbuildingBase, Health);
}

void AbuildingBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AbuildingBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AbuildingBase::UpdateMaterial(FLinearColor PlayerColor, AActor* BuildingRef) {
	ColorBldg(PlayerColor, BuildingRef);
}

void AbuildingBase::ColorBldg_Implementation(FLinearColor PlayerColor, AActor* BuildingRef) {
	BldgMeshMaterialDynamic->SetVectorParameterValue(ColorParameterText, PlayerColor);
	//@TODO: Try this instead
	//this->BuildingMesh->SetMaterial(0, BldgMeshMaterialDynamic);
	((AbuildingBase*)BuildingRef)->BuildingMesh->SetMaterial(0, BldgMeshMaterialDynamic);
}

bool AbuildingBase::ColorBldg_Validate(FLinearColor PlayerColor, AActor* BuildingRef) {
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