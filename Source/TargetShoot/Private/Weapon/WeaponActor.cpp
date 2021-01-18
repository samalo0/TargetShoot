// Copyright Stephen Maloney, 2020

#include "Weapon/WeaponActor.h"

#include "Animation/AnimSequence.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Weapon/ProjectileActor.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponActor::AWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(SkeletalMeshComponent);

#if WITH_EDITORONLY_DATA
	
	ProjectileSpawnArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnArrowComponent"));
	ProjectileSpawnArrowComponent->SetupAttachment(GetRootComponent());
	ProjectileSpawnArrowComponent->ArrowSize = 0.1f;

#endif
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(GetRootComponent());
	NiagaraComponent->SetAutoActivate(false);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("%s invalid world."), *GetNameSafe(this));
		return;
	}

	GameMode = Cast<ATargetShootGameModeBase>(World->GetAuthGameMode());
	if(!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("%s invalid game mode."), *GetNameSafe(this));
	}
}

void AWeaponActor::Fire() const
{
	if(!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("%s invalid game mode."), *GetNameSafe(this));
		return;
	}

	GameMode->PullActorFromPool(ProjectileClass, ProjectileSpawnRelativeTransform * GetActorTransform());

	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	NiagaraComponent->Activate(true);

	SkeletalMeshComponent->PlayAnimation(FireAnimation, false);
}

#if WITH_EDITOR

void AWeaponActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ProjectileSpawnRelativeTransform = ProjectileSpawnArrowComponent->GetRelativeTransform();
}

#endif

