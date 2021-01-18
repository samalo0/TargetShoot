// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "Core/PoolInterface.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

class UDataTable;
class UProjectileMovementComponent;

UCLASS()
class TARGETSHOOT_API AProjectileActor : public AActor, public IPoolInterface
{
	GENERATED_BODY()
	
public:	

	AProjectileActor();

	virtual void PoolInterface_DestroyOrReturnToPool() override;

	virtual void PoolInterface_Initialize() override;
	
	virtual void PoolInterface_OnPullFromPool(FTransform Transform) override;

	virtual void PoolInterface_OnReturnToPool() override;
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly)
	UDataTable* EffectDataTable;
	
	UPROPERTY(Transient, VisibleInstanceOnly)
	ATargetShootGameModeBase* GameMode = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float InitialSpeed = 10000.0f;
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

};
