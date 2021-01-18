// Copyright Stephen Maloney, 2020

#include "Weapon/HitEffectActor.h"

#include "NiagaraComponent.h"
#include "Core/TargetShootGameModeBase.h"

AHitEffectActor::AHitEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	SetRootComponent(NiagaraComponent);
}

void AHitEffectActor::BeginPlay()
{
	Super::BeginPlay();

	NiagaraComponent->OnSystemFinished.AddDynamic(this, &AHitEffectActor::OnSystemFinished);
}

void AHitEffectActor::OnSystemFinished(UNiagaraComponent* PSystem)
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
	NiagaraComponent->Activate(true);
}

void AHitEffectActor::PoolInterface_OnReturnToPool()
{
	NiagaraComponent->Deactivate();
}

