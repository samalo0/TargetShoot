// Copyright Stephen Maloney, 2020

#include "Weapon/HitEffectActor.h"

#include "Particles/ParticleSystemComponent.h"
#include "Core/TargetShootGameModeBase.h"

AHitEffectActor::AHitEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	SetRootComponent(ParticleSystemComponent);
}

void AHitEffectActor::BeginPlay()
{
	Super::BeginPlay();

	ParticleSystemComponent->OnSystemFinished.AddDynamic(this, &AHitEffectActor::OnSystemFinished);
}

void AHitEffectActor::OnSystemFinished(UParticleSystemComponent* PSystem)
{
	PoolInterface_DestroyOrReturnToPool();
}

void AHitEffectActor::PoolInterface_DestroyOrReturnToPool()
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

void AHitEffectActor::PoolInterface_Initialize()
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

void AHitEffectActor::PoolInterface_OnPullFromPool(const FTransform Transform)
{
	SetActorTransform(Transform);
	ParticleSystemComponent->Activate(true);
}

void AHitEffectActor::PoolInterface_OnReturnToPool()
{
	ParticleSystemComponent->Deactivate();
}

