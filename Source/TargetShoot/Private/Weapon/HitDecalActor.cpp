// Copyright Stephen Maloney, 2020

#include "Weapon/HitDecalActor.h"

#include "Components/DecalComponent.h"
#include "Core/TargetShootGameModeBase.h"
#include "Engine/World.h"

AHitDecalActor::AHitDecalActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	SetRootComponent(DecalComponent);
	DecalComponent->FadeScreenSize = 0.0f;
}

void AHitDecalActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(IsValid(World))
	{
		GameMode = Cast<ATargetShootGameModeBase>(World->GetAuthGameMode());
		if(IsValid(GameMode))
		{
			GameMode->RegisterDecal(this);		
		}
	}
}

void AHitDecalActor::PoolInterface_DestroyOrReturnToPool()
{
	if(bIsPooling && IsValid(GameMode))
	{
		GameMode->ReturnActorToPool(this);
	}
	else
	{
		Destroy();
	}
}

void AHitDecalActor::PoolInterface_Initialize()
{
	UWorld* World = GetWorld();
	if(IsValid(World))
	{
		GameMode = Cast<ATargetShootGameModeBase>(World->GetAuthGameMode());
		if(IsValid(GameMode))
		{
			bIsPooling = true;
		}
	}
}

void AHitDecalActor::PoolInterface_OnPullFromPool(const FTransform Transform)
{
	SetActorTransform(Transform);
	DecalComponent->SetHiddenInGame(false);
	if(IsValid(GameMode))
	{
		GameMode->RegisterDecal(this);
	}
}

void AHitDecalActor::PoolInterface_OnReturnToPool()
{
	if(IsValid(GameMode))
	{
		GameMode->UnregisterDecal(this);
	}
	DecalComponent->SetHiddenInGame(true);
}


