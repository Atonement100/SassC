// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/BuildingBase.h"
#include "SassC.h"
#include "Core/SassCStaticLibrary.h"
#include "Net/UnrealNetwork.h"

ABuildingBase::ABuildingBase()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 20.0f;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneComponent->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = SceneComponent;
	AActor::SetReplicateMovement(true);
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	bReplicates = true;
	
	ActiveBuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Active Building Mesh"));
	ActiveBuildingMesh->SetupAttachment(RootComponent);
	ActiveBuildingMesh->SetRelativeLocation(FVector(0, 0, 1));
	ActiveBuildingMesh->SetRelativeScale3D(FVector(USassCStaticLibrary::HammerToUnrealScalar())); //1.905 is the Hammer -> UE conversion
	
	BuildingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Building Collision"));
	BuildingCollision->SetupAttachment(RootComponent);
	BuildingCollision->SetBoxExtent(CollisionBounds);
	BuildingCollision->SetRelativeLocation(CollisionDisplacement);
	BuildingCollision->SetRelativeScale3D(FVector(1.0f));
}

void ABuildingBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuildingBase, Health);
	DOREPLIFETIME(ABuildingBase, OwningPlayerID);
	DOREPLIFETIME(ABuildingBase, OwningPlayerColor);
}

void ABuildingBase::PostActorCreated()
{
	Super::PostActorCreated();

	if (AvailableBuildingMeshes.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildingMeshes have not yet been set for %s"), *GetNameSafe(this));
	}
	else if (ActiveBuildingMesh)
	{
		if (AvailableBuildingMeshes[0]->IsA(UStaticMesh::StaticClass()))
		{
			Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[0]));
		}
		else if (AvailableBuildingMeshes[0]->IsA(USkeletalMesh::StaticClass()))
		{
			Cast<USkeletalMeshComponent>(ActiveBuildingMesh)->SetSkeletalMesh(Cast<USkeletalMesh>(AvailableBuildingMeshes[0]));
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("PostActorCreated"));
	if (!ActiveBuildingMesh)
		UE_LOG(LogTemp, Display, TEXT("Not available in PostActorCreated"));
}

void ABuildingBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UE_LOG(LogTemp, Display, TEXT("Number of components is %d!"), AvailableBuildingMeshes.Num());

	if (!ActiveBuildingMesh)
		UE_LOG(LogTemp, Display, TEXT("Not available in PreInitialize"));
}

void ABuildingBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogTemp, Display, TEXT("PostInitialize"));

	if (!ActiveBuildingMesh)
		UE_LOG(LogTemp, Display, TEXT("Not available in PostInitialize"));

	//todo: confirm that applying dynamic material to main mesh will apply when replacing model w upgrades
	if (ActiveBuildingMesh)
	{
		for (int Index = 0; Index < ActiveBuildingMesh->GetMaterials().Num(); Index++)
		{
			UMaterialInterface* Material = ActiveBuildingMesh->GetMaterial(Index);
			BldgMeshMaterialDynamic.Add(ActiveBuildingMesh->CreateDynamicMaterialInstance(Index, Material));
		}
	}

	//These are to include the additional meshes and materials in the dynamic material array, parent only adds one mesh and material by default.
	// BldgMeshMaterialDynamic.Add(BuildingMesh->CreateDynamicMaterialInstance(1, BuildingMesh->GetMaterial(1)));
	// for (int NumMaterials = 0; NumMaterials < UpgradeOneMesh->GetNumMaterials(); NumMaterials++)
	// {
	// 	BldgMeshMaterialDynamic.Add(
	// 		UpgradeOneMesh->CreateDynamicMaterialInstance(NumMaterials, UpgradeOneMesh->GetMaterial(NumMaterials)));
	// }
	// for (int NumMaterials = 0; NumMaterials < UpgradeOneMesh->GetNumMaterials(); NumMaterials++)
	// {
	// 	BldgMeshMaterialDynamic.Add(
	// 		UpgradeTwoMesh->CreateDynamicMaterialInstance(NumMaterials, UpgradeTwoMesh->GetMaterial(NumMaterials)));
	// }
	//This line is to include the second mesh in the dynamic material array, parent only allows for one mesh by default.
	// BldgMeshMaterialDynamic.Add(UpgradeOneMesh->CreateDynamicMaterialInstance(0, UpgradeOneMesh->GetMaterial(0)));
}

void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABuildingBase::UpdateMaterial(FLinearColor PlayerColor, bool SetPersistentColor)
{
	ColorBldg(PlayerColor, SetPersistentColor);
}

void ABuildingBase::ColorBldg_Implementation(FLinearColor PlayerColor, bool SetPersistentColor)
{
	for (UMaterialInstanceDynamic* Material : BldgMeshMaterialDynamic)
	{
		if (Material) Material->SetVectorParameterValue(ColorParameterText, PlayerColor);
	}
	if (SetPersistentColor) { this->OwningPlayerColor = PlayerColor; }
}

bool ABuildingBase::ColorBldg_Validate(FLinearColor PlayerColor, bool SetPersistentColor)
{
	return true;
}

float ABuildingBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= DamageAmount;
	if (Health <= 0.0f)
	{
		SetLifeSpan(0.001f);
	}
	return DamageAmount;
}

float ABuildingBase::GetHealth()
{
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

UMeshComponent* ABuildingBase::GetMesh(int MeshIndex)
{
	return this->ActiveBuildingMesh;
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
	PreviewActive = false;
	const short LevelToPreview = UpgradeLevel + 1;
	
	if (!AvailableBuildingMeshes.IsValidIndex(LevelToPreview))
	{
		UE_LOG(LogTemp, Display, TEXT("PreviewUpgrade_Implementation - Already at maximum level."));
		return;		
	}

	ResetRequired = true;
	
	Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[LevelToPreview]));
	this->ColorBldg(FLinearColor::Green);
}

void ABuildingBase::ResetPreview_Implementation()
{
	ResetRequired = false;
	
	if (!AvailableBuildingMeshes.IsValidIndex(UpgradeLevel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Upgrade level %d is out of range of available building mesh."), UpgradeLevel);
		return;		
	}
	
	Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[UpgradeLevel]));
	this->ColorBldg(OwningPlayerColor);
}

void ABuildingBase::UpgradeBuilding_Implementation()
{
	NetUpgradeBuilding();
}

bool ABuildingBase::UpgradeBuilding_Validate()
{
	return true;
}

void ABuildingBase::NetUpgradeBuilding_Implementation()
{
	Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[UpgradeLevel + 1]));
	this->ColorBldg(OwningPlayerColor);
	UpgradeLevel++;
}

bool ABuildingBase::NetUpgradeBuilding_Validate()
{
	return true;
}
#pragma endregion
