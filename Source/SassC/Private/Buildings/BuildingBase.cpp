// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/BuildingBase.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gamemode/Sassilization/SassPlayerState.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Engine/StreamableRenderAsset.h"
#include "Core/SassCStaticLibrary.h"
#include "Gamemode/Sassilization/SassGameManager.h"
#include "Gamemode/Sassilization/SassGameState.h"
#include "Kismet/KismetSystemLibrary.h"
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
		UE_LOG(Sassilization, Warning, TEXT("BuildingMeshes have not yet been set for %s"), *GetNameSafe(this));
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
}

void ABuildingBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ABuildingBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (ActiveBuildingMesh)
	{
		for (int Index = 0; Index < ActiveBuildingMesh->GetMaterials().Num(); Index++)
		{
			UMaterialInterface* Material = ActiveBuildingMesh->GetMaterial(Index);
			BldgMeshMaterialDynamic.Add(ActiveBuildingMesh->CreateDynamicMaterialInstance(Index, Material));
		}
	}
}

void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABuildingBase::WhenBuilt()
{
	UE_LOG(Sassilization, Display, TEXT("Triggering WhenBuilt for %s"), *this->GetName());

	IEntityInterface::WhenBuilt();

	const ASassGameManager* GameManager = GetWorld()->GetGameState<ASassGameState>()->GetGameManager();

	UE_LOG(Sassilization, Display, TEXT("GameManager %s"), *GameManager->GetName());
	
	AGraphNode* ClosestNode = GameManager->GetClosestNode(GetActorLocation(), 30.f);

	UE_LOG(Sassilization, Display, TEXT("WhenBuilt Closest Node %s Controlling Empire %s"), *ClosestNode->GetName(), *GetNameSafe(ControllingEmpire));
	
	this->TerritoryInfo = FTerritoryInfo();
	TerritoryInfo.Influence = this->GetInfluence();
	TerritoryInfo.EmpireId = ControllingEmpire->GetEmpireId();
	TerritoryInfo.ParentGraphNode = ClosestNode;

	UE_LOG(Sassilization, Display, TEXT("TerritoryInfo %s"), *this->TerritoryInfo.ToString());

	this->bIsBuilt = true;
	GameManager->UpdateTerritories();
}

void ABuildingBase::Initialize(AEmpire* NewEmpire)
{
	this->SetControl(NewEmpire);
}

void ABuildingBase::SetControl(AEmpire* NewEmpire)
{
	this->ControllingEmpire = NewEmpire;
	this->ColorBuilding(NewEmpire->GetColor(), true);
}

void ABuildingBase::UpdateMaterial(FLinearColor PlayerColor, bool SetPersistentColor)
{
	ColorBuilding(PlayerColor, SetPersistentColor);
}

void ABuildingBase::ColorBuilding_Implementation(FLinearColor PlayerColor, bool SetPersistentColor)
{
	for (UMaterialInstanceDynamic* Material : BldgMeshMaterialDynamic)
	{
		if (Material) Material->SetVectorParameterValue(ColorParameterText, PlayerColor);
	}
	if (SetPersistentColor) { this->OwningPlayerColor = PlayerColor; }
}

bool ABuildingBase::ColorBuilding_Validate(FLinearColor PlayerColor, bool SetPersistentColor)
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

FTerritoryInfo ABuildingBase::GetTerritoryInfo() const
{
	return TerritoryInfo;
}

void ABuildingBase::SetTerritoryInfo(const FTerritoryInfo& NewTerritoryInfo)
{
	this->TerritoryInfo = NewTerritoryInfo;
}

bool ABuildingBase::IsDestroyed() const
{
	return bIsDestroyed;
}

void ABuildingBase::SetIsDestroyed(const bool bNewIsDestroyed)
{
	this->bIsDestroyed = bNewIsDestroyed;
}

void ABuildingBase::NetFixSpawnLocation_Implementation(FVector RealLocation)
{
	SetRole(ROLE_Authority);
	//This is a horrible thing to do, but spawns are not sent to client with precision beyond int. 
	//I can only pray that the buildings do not rise up and attempt to take over servers. If that happens, we may all be doomed.
	//SetActorLocation(RealLocation + FVector(10));
	SetActorLocation(RealLocation);
}

void ABuildingBase::Expand(const FRotator DirectionToExpand)
{
	GetWorld()->GetGameState<ASassGameState>()->GetGameManager()->ServerRequestCityExpansion(this, DirectionToExpand);
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


void ABuildingBase::PreviewUpgrade()
{
	PreviewActive = false;
	const short LevelToPreview = UpgradeLevel + 1;
	
	if (!AvailableBuildingMeshes.IsValidIndex(LevelToPreview))
	{
		UE_LOG(Sassilization, Display, TEXT("PreviewUpgrade_Implementation - Already at maximum level."));
		return;		
	}

	ResetRequired = true;
	
	Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[LevelToPreview]));
	this->ColorBuilding(FLinearColor::Green);
}

void ABuildingBase::ResetPreview()
{
	ResetRequired = false;
	
	if (!AvailableBuildingMeshes.IsValidIndex(UpgradeLevel))
	{
		UE_LOG(Sassilization, Warning, TEXT("Upgrade level %d is out of range of available building mesh."), UpgradeLevel);
		return;		
	}
	
	Cast<UStaticMeshComponent>(ActiveBuildingMesh)->SetStaticMesh(Cast<UStaticMesh>(AvailableBuildingMeshes[UpgradeLevel]));
	this->ColorBuilding(OwningPlayerColor);
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
	this->ColorBuilding(OwningPlayerColor);
	UpgradeLevel++;
}

bool ABuildingBase::NetUpgradeBuilding_Validate()
{
	return true;
}
#pragma endregion
