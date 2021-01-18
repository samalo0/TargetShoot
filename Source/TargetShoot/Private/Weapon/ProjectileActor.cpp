// Copyright Stephen Maloney, 2020

#include "Weapon/ProjectileActor.h"

#include "Engine/DataTable.h"
#include "NiagaraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/TargetShootGameModeBase.h"
#include "Core/TargetShootStructs.h"
#include "Kismet/GameplayStatics.h"

AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->bReturnMaterialOnMove = true;
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &AProjectileActor::OnHit);

	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitialSpeed;
}

void AProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, const FHitResult& Hit)
{
	if(IsValid(EffectDataTable))
	{
		UPhysicalMaterial* PhysicalMaterial = Hit.PhysMaterial.Get();
		if(IsValid(PhysicalMaterial))
		{
			const FName Name = FName(*PhysicalMaterial->GetName());
			FMaterialEffectRow* EffectRow = EffectDataTable->FindRow<FMaterialEffectRow>(Name, "CollisionAudio");
			if(EffectRow != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, EffectRow->Sound, GetActorLocation());

				if(IsValid(GameMode) && IsValid(EffectRow->ActorToSpawnOnHit))
				{
					const FTransform Transform = FTransform(Hit.ImpactNormal.ToOrientationRotator(), Hit.ImpactPoint);
					GameMode->PullActorFromPool(EffectRow->ActorToSpawnOnHit, Transform);
				}
			}			
		}
	}
	
	PoolInterface_DestroyOrReturnToPool();
}

void AProjectileActor::PoolInterface_DestroyOrReturnToPool()
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

void AProjectileActor::PoolInterface_Initialize()
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

void AProjectileActor::PoolInterface_OnPullFromPool(const FTransform Transform)
{
	SetActorTransform(Transform);
	StaticMeshComponent->SetHiddenInGame(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitialSpeed;
	ProjectileMovementComponent->Activate(true);
}

void AProjectileActor::PoolInterface_OnReturnToPool()
{
	ProjectileMovementComponent->Deactivate();
	StaticMeshComponent->SetHiddenInGame(true);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

